/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageResource.h"
#include "FileSystem.h"
#include <PixelConversion.h>
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <stb_image.h>
#include <fmt/format.h>

using Napi::AsyncTask;
using Napi::EscapableHandleScope;
using Napi::Number;
using Napi::Object;
using Napi::String;
using Napi::Value;
using Napi::ObjectGetString;
using Napi::ObjectGetStringOrEmpty;
using Napi::ObjectGetNumberOrDefault;

namespace ls {

using NSVGimageHandle = std::unique_ptr<NSVGimage, decltype(&nsvgDelete)>;
using NSVGrasterizerHandle = std::unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)>;

float ScaleFactor(const int, const int);
std::shared_ptr<ImageInfo> DecodeImage(const ImageUri&, const std::vector<std::string>&,
    const std::vector<std::string>&, PixelFormat);
std::shared_ptr<ImageInfo> DecodeImageSvg(NSVGimage*, const std::string, const int32_t, const int32_t);

ImageResource::ImageResource(Napi::Env env, const ImageUri& uri) : Resource(env, uri.GetId()), uri(uri) {
}

bool ImageResource::Sync(Renderer* renderer) {
    if (this->textureId) {
        return true;
    }

    if (!this->IsReady() || !this->image) {
        return false;
    }

    this->textureId = renderer->CreateTexture(
        this->image->data.get(),
        this->image->format,
        this->image->width,
        this->image->height);

    this->image.reset();

    return (this->textureId != 0);
}

void ImageResource::Load(Renderer* renderer,
        const std::vector<std::string>& extensions, const std::vector<std::string>& resourcePath) {
    auto initialState{ ResourceStateLoading };
    const auto textureFormat{ renderer->GetTextureFormat() };
    auto uri{ this->uri };

    try {
        this->task = std::make_unique<AsyncTask<ImageInfo>>(
            this->env,
            this->id,
            [uri, extensions, resourcePath, textureFormat]() -> std::shared_ptr<ImageInfo> {
                return DecodeImage(uri, extensions, resourcePath, textureFormat);
            },
            [this](Napi::Env env, std::shared_ptr<ImageInfo> result, napi_status status, const std::string& message) {
                // TODO: assert(this->resourceState != ResourceStateLoading)
                // TODO: assert(this->GetRefCount() > 0)

                this->image = result;

                if (this->image) {
                    this->width = this->image->width;
                    this->height = this->image->height;
                } else {
                    this->width = this->height = 0;
                }

                this->task.reset();

                fmt::println("image load: width {} height {} status {} '{}'", width, height, status, message);

                this->SetStateAndNotifyListeners(status != napi_ok ? ResourceStateError : ResourceStateReady);
            });
    } catch (std::exception& e) {
        initialState = ResourceStateError;
    }

    this->SetStateAndNotifyListeners(initialState);
}

std::shared_ptr<ImageInfo> DecodeImage(const ImageUri& uri, const std::vector<std::string>& extensions,
             const std::vector<std::string>& resourcePath, PixelFormat textureFormat) {
    std::shared_ptr<ImageInfo> result;
    auto uriOrFilename{ uri.GetUri() };

    // TODO: support base64 encoded svgs
    // TODO: support url encoding

    if (IsDataUri(uriOrFilename)) {
        if (!IsSvgDataUri(uriOrFilename)) {
            throw std::runtime_error(fmt::format("Invalid image data uri: {}", uriOrFilename));
        }

        auto dataString{ GetSvgUriData(uriOrFilename) };

        result = DecodeImageSvg(
            nsvgParse(const_cast<char *>(dataString.data()), "px", 96),
            uriOrFilename,
            uri.GetWidth(),
            uri.GetHeight());
    } else {
        std::string filename;

        if (IsResourceUri(uriOrFilename)) {
            filename = FindFile(GetResourceUriPath(uriOrFilename), extensions, resourcePath);
        } else {
            filename = FindFile(uriOrFilename, extensions);
        }

        FileHandle file(fopen(filename.c_str(), "rb"), fclose);

        if (!file) {
            throw std::runtime_error(fmt::format("Could not open image file: {}", uriOrFilename));
        }

        int32_t components{};
        int32_t width{};
        int32_t height{};

        auto data{ stbi_load_from_file(file.get(), &width, &height, &components, 4) };

        if (data) {
            result = std::make_shared<ImageInfo>();
            result->width = width,
            result->height = height,
            result->data = std::shared_ptr<uint8_t>(data, [] (uint8_t* p) { stbi_image_free(p); });
            result->format = PixelFormatRGBA;
        } else {
            fseek(file.get(), 0, SEEK_SET);

            result = DecodeImageSvg(
                nsvgParseFromFilePtr(file.get(), "px", 96),
                uriOrFilename,
                uri.GetWidth(),
                uri.GetHeight());
        }
    }

    if (result) {
        ConvertToFormat(result->data.get(), result->width * result->height * 4, textureFormat);
        result->format = textureFormat;
    }

    return result;
}

std::shared_ptr<ImageInfo> DecodeImageSvg(NSVGimage* svgImage, const std::string& uri, const int32_t scaleWidth,
        const int32_t scaleHeight) {
    NSVGimageHandle svg(svgImage, nsvgDelete);

    // XXX: nsvgParse* methods do not check if parsing failed. NSVGImage can be left in a partially filled out state,
    // resulting in a bad render. Negative width and height are an indication that parsing failed, but that does not
    // cover all invalid XML use cases.

    if (svg == nullptr) {
        throw std::runtime_error(fmt::format("Failed to parse svg image: {}", uri));
    }

    auto svgWidth{ static_cast<int32_t>(svg->width) };
    auto svgHeight{ static_cast<int32_t>(svg->height) };

    if (svgWidth < 0 || svgHeight < 0) {
        throw std::runtime_error(fmt::format("Failed to parse svg image: {}", uri));
    }

    if ((svgWidth == 0 || svgHeight == 0) && (scaleWidth == 0 && scaleHeight == 0)) {
        throw std::runtime_error(fmt::format("No dimensions available for svg image: {}", uri));
    }

    auto scaleX{ 1.f };
    auto scaleY{ 1.f };
    int32_t width;
    int32_t height;

    if (scaleWidth > 0 && scaleHeight > 0) {
        width = scaleWidth;
        height = scaleHeight;
        scaleX = ScaleFactor(svgWidth, scaleWidth);
        scaleY = ScaleFactor(svgHeight, scaleHeight);
    } else {
        width = svgWidth;
        height = svgHeight;
        scaleX = 1.0f;
        scaleY = 1.0f;
    }

    NSVGrasterizerHandle rasterizer(nsvgCreateRasterizer(), nsvgDeleteRasterizer);

    if (rasterizer == nullptr) {
        throw std::runtime_error(fmt::format("Failed to create rasterizer for svg image: {}", uri));
    }

    std::shared_ptr<uint8_t> data(new uint8_t [width * height * 4], [](uint8_t* p){ delete [] p; });

    nsvgRasterizeFull(rasterizer.get(),
                      svg.get(),
                      0,
                      0,
                      scaleX,
                      scaleY,
                      data.get(),
                      width,
                      height,
                      width * 4);

    auto image{ std::make_shared<ImageInfo>() };

    image->width = width,
    image->height = height,
    image->data = data;
    image->format = PixelFormatRGBA;

    return image;
}

EdgeRect ToCapInsets(const Object& spec) {
    return {
        ObjectGetNumberOrDefault(spec, "top", 0),
        ObjectGetNumberOrDefault(spec, "right", 0),
        ObjectGetNumberOrDefault(spec, "bottom", 0),
        ObjectGetNumberOrDefault(spec, "left", 0),
    };
}

ImageUri ImageUri::FromObject(const Object& spec) {
    auto uri{ ObjectGetString(spec, "uri") };
    auto width{ ObjectGetNumberOrDefault(spec, "width", 0) };
    auto height{ ObjectGetNumberOrDefault(spec, "height", 0) };
    auto id{ ObjectGetStringOrEmpty(spec, "id") };

    if (width < 0) {
        width = 0;
    }

    if (height < 0) {
        height = 0;
    }

    if (spec.Has("capInsets") && spec.Get("capInsets").IsObject()) {
        return ImageUri(uri, id, width, height, ToCapInsets(spec.Get("capInsets").As<Object>()));
    }

    return ImageUri(uri, id, width, height);
}

Value ImageUri::ToObject(Napi::Env env) {
    EscapableHandleScope scope(env);
    auto imageUri{ Object::New(env) };

    imageUri["id"] = String::New(env, this->GetId());
    imageUri["uri"] = String::New(env, this->GetUri());

    if (this->width > 0) {
        imageUri["width"] = Number::New(env, this->width);
    }

    if (this->height > 0) {
        imageUri["height"] = Number::New(env, this->height);
    }

    if (this->HasCapInsets()) {
        auto capInsets{ Object::New(env) };

        capInsets["top"] = Number::New(env, this->capInsets.top);
        capInsets["right"] = Number::New(env, this->capInsets.right);
        capInsets["bottom"] = Number::New(env, this->capInsets.bottom);
        capInsets["left"] = Number::New(env, this->capInsets.left);

        imageUri["capInsets"] = capInsets;
    }

    return scope.Escape(imageUri);
}

inline
float ScaleFactor(const int source, const int dest) {
    return 1.f + ((dest - source) / static_cast<float>(source));
}

} // namespace ls
