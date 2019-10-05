/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageResource.h"
#include "FileSystem.h"
#include <napi-ext.h>
#include <PixelConversion.h>
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <stb_image.h>
#include <algorithm>
#include <fmt/println.h>

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
std::shared_ptr<void> DecodeImage(const ImageUri&, const std::vector<std::string>&,
    const std::vector<std::string>&, const PixelFormat);
std::shared_ptr<ImageInfo> DecodeImageSvg(NSVGimage* svgImage, const std::string& uri, const int32_t scaleWidth,
        const int32_t scaleHeight);

ImageResource::ImageResource(const ImageUri& uri) noexcept : Resource(uri.GetId()), uri(uri) {
}

ImageResource::~ImageResource() noexcept {
    if (this->task) {
        this->task->Cancel();
    }
}

bool ImageResource::Sync(Renderer* renderer) {
    if (this->textureId) {
        return true;
    }

    if (!this->IsReady() || !this->image) {
        return false;
    }

    this->textureId = renderer->CreateTexture(
        this->image->width,
        this->image->height,
        this->image->data.get(),
        this->image->format);

    this->image.reset();

    return (this->textureId != 0);
}

void ImageResource::Load(AsyncTaskQueue* taskQueue, Renderer* renderer,
        const std::vector<std::string>& extensions, const std::vector<std::string>& resourcePath) {
    assert(!this->task);

    auto initialState{ ResourceStateLoading };
    const auto textureFormat{ renderer->GetTextureFormat() };
    const auto uri{ this->uri };

    auto execute = [uri, extensions, resourcePath, textureFormat]() -> std::shared_ptr<void> {
        return DecodeImage(uri, extensions, resourcePath, textureFormat);
    };

    auto complete = [this](std::shared_ptr<Task> task) {
        if (this->resourceState != ResourceStateLoading || task != this->task) {
            return;
        }

        ResourceState nextState;

        if (task->IsError()) {
            this->image.reset();
            this->width = this->height = 0;
            nextState = ResourceStateError;

            fmt::println("image load: Error: {}", task->GetErrorMessage());
        } else {
            this->image = task->GetResultAs<ImageInfo>();
            this->width = this->image->width;
            this->height = this->image->height;
            nextState = ResourceStateReady;

            fmt::println("image load: {}x{}", width, height);
        }

        this->task.reset();
        this->SetStateAndNotifyListeners(nextState);
    };

    try {
        this->task = taskQueue->Submit(std::move(execute), std::move(complete));
    } catch (const std::exception&) {
        initialState = ResourceStateError;
    }

    this->SetStateAndNotifyListeners(initialState);
}

Value ImageResource::ToObject(const Napi::Env& env) const {
    EscapableHandleScope scope(env);
    auto font{ Object::New(env) };

    font["uri"] = this->GetUri().ToObject(env);
    font["resourceId"] = String::New(env, this->GetId());
    font["refs"] = Number::New(env, this->GetRefCount());
    font["state"] = String::New(env, ResourceStateToString(this->resourceState));

    return scope.Escape(font);
}

void ImageResource::Detach(Renderer* renderer) {
    if (resourceState == ResourceStateReady) {
        if (this->textureId) {
            renderer->DestroyTexture(this->textureId);
            this->textureId = 0;
        }

        resourceState = ResourceStateInit;
    }
}

std::shared_ptr<void> DecodeImage(const ImageUri& uri, const std::vector<std::string>& extensions,
             const std::vector<std::string>& resourcePath, const PixelFormat textureFormat) {
    std::shared_ptr<ImageInfo> result;
    const auto& uriOrFilename{ uri.GetUri() };

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
        const fs::path filename {
            IsResourceUri(uriOrFilename) ?
                FindFile(GetResourceUriPath(uriOrFilename), extensions, resourcePath)
                    : FindFile(uriOrFilename, extensions)
        };
        const FileHandle file(fopen(filename.c_str(), "rb"), fclose);

        if (!file) {
            throw std::runtime_error(fmt::format("Could not open image file: {}", uriOrFilename));
        }

        int32_t components{};
        int32_t width{};
        int32_t height{};

        const auto data{ stbi_load_from_file(file.get(), &width, &height, &components, 4) };

        if (data) {
            result = std::make_shared<ImageInfo>(
                width,
                height,
                PixelFormatRGBA,
                std::shared_ptr<uint8_t>(data, [] (uint8_t* p) { stbi_image_free(p); }));
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

    return std::static_pointer_cast<void>(result);
}

std::shared_ptr<ImageInfo> DecodeImageSvg(NSVGimage* svgImage, const std::string& uri, const int32_t scaleWidth,
        const int32_t scaleHeight) {
    const NSVGimageHandle svg(svgImage, nsvgDelete);

    // XXX: nsvgParse* methods do not check if parsing failed. NSVGImage can be left in a partially filled out state,
    // resulting in a bad render. Negative width and height are an indication that parsing failed, but that does not
    // cover all invalid XML use cases.

    if (svg == nullptr) {
        throw std::runtime_error(fmt::format("Failed to parse svg image: {}", uri));
    }

    const auto svgWidth{ static_cast<int32_t>(svg->width) };
    const auto svgHeight{ static_cast<int32_t>(svg->height) };

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

    const NSVGrasterizerHandle rasterizer(nsvgCreateRasterizer(), nsvgDeleteRasterizer);

    if (rasterizer == nullptr) {
        throw std::runtime_error(fmt::format("Failed to create rasterizer for svg image: {}", uri));
    }

    const std::shared_ptr<uint8_t> data(new uint8_t [width * height * 4], [](uint8_t* p){ delete [] p; });

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

    return std::make_shared<ImageInfo>(width, height, PixelFormatRGBA, data);
}

EdgeRect ToCapInsets(const Object& spec) {
    return {
        ObjectGetNumberOrDefault(spec, "top", 0),
        ObjectGetNumberOrDefault(spec, "right", 0),
        ObjectGetNumberOrDefault(spec, "bottom", 0),
        ObjectGetNumberOrDefault(spec, "left", 0),
    };
}

ImageUri::ImageUri(const std::string& uri) noexcept : uri(uri) {
}

ImageUri::ImageUri(const std::string& uri, const std::string& id, const int32_t width, const int32_t height) noexcept
: uri(uri), id(id), width(width), height(height) {
}

ImageUri::ImageUri(
    const std::string& uri,
    const std::string& id,
    const int32_t width,
    const int32_t height,
    const EdgeRect& capInsets) noexcept
: uri(uri), id(id), width(width), height(height), capInsets(capInsets),
      hasCapInsets(capInsets.top || capInsets.right || capInsets.bottom || capInsets.left) {
}

ImageUri ImageUri::FromObject(const Object& spec) {
    const auto uri{ ObjectGetString(spec, "uri") };
    const auto id{ ObjectGetStringOrEmpty(spec, "id") };
    const auto width{ std::max(ObjectGetNumberOrDefault(spec, "width", 0), 0) };
    const auto height{ std::max(ObjectGetNumberOrDefault(spec, "height", 0), 0) };

    if (spec.Has("capInsets") && spec.Get("capInsets").IsObject()) {
        return ImageUri(uri, id, width, height, ToCapInsets(spec.Get("capInsets").As<Object>()));
    }

    return ImageUri(uri, id, width, height);
}

Value ImageUri::ToObject(const Napi::Env& env) const {
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

ImageInfo::ImageInfo(const int32_t width, const int32_t height, const PixelFormat format,
        const std::shared_ptr<uint8_t>& data) noexcept
    : width(width), height(height), format(format), data(data) {
}

inline
float ScaleFactor(const int source, const int dest) {
    return 1.f + ((dest - source) / static_cast<float>(source));
}

} // namespace ls
