/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageResource.h"
#include <napi-ext.h>
#include <ls/FileSystem.h>
#include <ls/Renderer.h>
#include <ls/PixelConversion.h>
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <stb_image.h>
#include <algorithm>
#include <fmt/println.h>
#include "Scene.h"
#include "Stage.h"

using Napi::Boolean;
using Napi::EscapableHandleScope;
using Napi::Object;
using Napi::String;
using Napi::Value;

namespace ls {

using NSVGimageHandle = std::unique_ptr<NSVGimage, decltype(&nsvgDelete)>;
using NSVGrasterizerHandle = std::unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)>;

static inline float ScaleFactor(const int, const int) noexcept;
static Surface DecodeImage(const ImageUri&, const std::vector<std::string>&, const std::vector<std::string>&,
    const PixelFormat);
static Surface DecodeImageSvg(NSVGimage*, const std::string&, const int32_t, const int32_t);

ImageResource::ImageResource(const ImageUri& uri) noexcept : Resource(uri.GetId()), uri(uri) {
}

ImageResource::~ImageResource() noexcept {
    this->loadImageTask.Cancel();
}

bool ImageResource::Sync(Renderer* renderer) {
    if (this->textureId) {
        return true;
    }

    if (!this->IsReady() && !this->image.IsEmpty()) {
        return false;
    }

    if (this->image.Format() != renderer->GetTextureFormat()) {
        this->image.Convert(renderer->GetTextureFormat());
    }

    this->textureId = renderer->CreateTexture(image);
    this->image = Surface();

    return (this->textureId != 0);
}

void ImageResource::Attach(Scene* scene) {
    this->scene = scene;

    if (this->GetState() == ResourceStateInit) {
        this->Load();
    }
}

void ImageResource::Detach() {
    this->loadImageTask.Cancel();

    if (this->GetState() != ResourceStateError) {
        this->SetState(ResourceStateInit, false);
    }

    this->scene->GetRenderer()->DestroyTexture(this->textureId);

    this->scene = nullptr;
}

void ImageResource::Load() {
    assert(this->GetState() == ResourceStateInit);

    const auto stage{ this->scene->GetStage() };

    this->loadImageTask.Cancel();

    auto execute = [
            uri = this->uri,
            extensions = this->scene->GetImageStore()->GetSearchExtensions(),
            resourcePath = stage->GetResourcePath(),
            textureFormat = this->scene->GetRenderer()->GetTextureFormat()]() -> Surface {
        return DecodeImage(uri, extensions, { resourcePath }, textureFormat);
    };

    auto callback = [this](Surface&& surface, const std::exception_ptr& eptr) {
        if (eptr) {
            this->width = this->height = 0;

            try {
                std::rethrow_exception(eptr);
            } catch (const std::exception& e) {
                fmt::println("image load: Error: {} {}", this->uri.GetId(), e.what());
            }

            this->SetState(ResourceStateError, true);
        } else {
            this->width = surface.Width();
            this->height = surface.Height();
            this->image = std::move(surface);

            fmt::println("image load: {} {}x{}", this->uri.GetId(), this->width, this->height);

            this->SetState(ResourceStateReady, true);
        }
    };

    auto initialState{ ResourceStateLoading };

    try {
        this->loadImageTask = stage->GetTaskQueue()->Async<Surface>(std::move(execute), std::move(callback));
    } catch (const std::exception&) {
        initialState = ResourceStateError;
    }

    this->SetState(initialState, true);
}

Value ImageResource::ToObject(const Napi::Env& env) const {
    EscapableHandleScope scope(env);
    auto image{ this->GetUri().ToObject(env).As<Object>() };

    image["state"] = String::New(env, ResourceStateToString(this->resourceState));
    image["hasTexture"] = Boolean::New(env, this->textureId > 0);

    return scope.Escape(image);
}

static Surface DecodeImage(const ImageUri& uri, const std::vector<std::string>& extensions,
             const std::vector<std::string>& resourcePath, const PixelFormat textureFormat) {
    Surface image;
    const auto& uriOrFilename{ uri.GetUri() };

    // TODO: support base64 encoded svgs
    // TODO: support url encoding

    if (IsDataUri(uriOrFilename)) {
        if (!IsSvgDataUri(uriOrFilename)) {
            throw std::runtime_error(fmt::format("Invalid image data uri: {}", uriOrFilename));
        }

        const auto dataString{ GetSvgUriData(uriOrFilename) };
        const auto svg{ nsvgParse(const_cast<char *>(dataString.data()), "px", 96) };

        image = DecodeImageSvg(svg, uriOrFilename, uri.GetWidth(), uri.GetHeight());
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
            image = {
                std::shared_ptr<uint8_t>(data, [] (uint8_t* p) { stbi_image_free(p); }),
                width,
                height,
                width * 4,
                PixelFormatRGBA
            };
        } else {
            fseek(file.get(), 0, SEEK_SET);

            const auto svg{ nsvgParseFromFilePtr(file.get(), "px", 96) };

            image = DecodeImageSvg(svg, uriOrFilename, uri.GetWidth(), uri.GetHeight());
        }
    }

    if (textureFormat != PixelFormatUnknown) {
        image.Convert(textureFormat);
    }

    return image;
}

static Surface DecodeImageSvg(NSVGimage* svgImage, const std::string& uri, const int32_t scaleWidth,
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

    const auto pitch{ width * 4 };
    const std::shared_ptr<uint8_t> data(new uint8_t [pitch * height], [](uint8_t* p){ delete [] p; });

    nsvgRasterizeFull(rasterizer.get(),
                      svg.get(),
                      0,
                      0,
                      scaleX,
                      scaleY,
                      data.get(),
                      width,
                      height,
                      pitch);

    return { data, width, height, pitch, PixelFormatRGBA };
}

static inline
float ScaleFactor(const int source, const int dest) noexcept {
    return 1.f + ((dest - source) / static_cast<float>(source));
}

} // namespace ls
