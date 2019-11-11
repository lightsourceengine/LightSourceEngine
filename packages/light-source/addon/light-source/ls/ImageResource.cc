/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageResource.h"
#include "Scene.h"
#include "Stage.h"
#include <napi-ext.h>
#include <ls/FileSystem.h>
#include <ls/Renderer.h>
#include <ls/PixelConversion.h>
#include <ls/Log.h>
#include <ls/Format.h>
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <stb_image.h>
#include <algorithm>
#include <cctype>
#include <std20/bit>

using Napi::Boolean;
using Napi::EscapableHandleScope;
using Napi::Object;
using Napi::String;
using Napi::Value;
using std20::endian;

namespace ls {

using NSVGimageHandle = std::unique_ptr<NSVGimage, decltype(&nsvgDelete)>;
using NSVGrasterizerHandle = std::unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)>;

static float ScaleFactor(const int, const int) noexcept;
static Surface DecodeImage(const ImageUri&, const std::vector<std::string>&, const std::vector<std::string>&,
    const PixelFormat);
static Surface DecodeImageSvg(NSVGimage*, const std::string&, const int32_t, const int32_t);
static bool HasSvgHeader(const CFile&);

// nanosvg and stb_image both output to this format
constexpr auto rawImagePixelFormat{ endian::native == endian::big ? PixelFormatRGBA : PixelFormatABGR };

ImageResource::ImageResource(const ImageUri& uri) noexcept : Resource(uri.GetId()), uri(uri) {
}

ImageResource::~ImageResource() noexcept {
    this->loadImageTask.Cancel();
}

bool ImageResource::Sync(Renderer* renderer) {
    if (this->texture) {
        return true;
    }

    if (!this->IsReady() && this->image.IsEmpty()) {
        return false;
    }

    if (this->image.Format() != renderer->GetTextureFormat()) {
        this->image.Convert(renderer->GetTextureFormat());
    }

    this->texture = renderer->CreateTextureFromSurface(image);
    this->image = {};

    auto result{ this->texture != nullptr && this->texture->IsAttached() };

    if (!result) {
        LOG_WARN("Failed to creat texture from image resource %s", this->GetId());
    }

    return result;
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

    this->texture = nullptr;
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

    auto callback = [this, LAMBDA_FUNCTION = __FUNCTION__](Surface& surface, const std::exception_ptr& eptr) {
        if (eptr) {
            this->width = this->height = 0;
            this->aspectRatio = 0;

            try {
                std::rethrow_exception(eptr);
            } catch (const std::exception& e) {
                LOG_ERROR_LAMBDA(e);
            }

            this->SetState(ResourceStateError, true);
        } else {
            this->width = surface.Width();
            this->height = surface.Height();
            this->aspectRatio = this->height > 0 ?  this->GetWidthF() / this->GetHeightF() : 0.f;
            this->image = std::move(surface);

            LOG_INFO_LAMBDA("%s: %s", ResourceStateToString(ResourceStateReady), this->GetId());

            this->SetState(ResourceStateReady, true);
        }
    };

    try {
        this->loadImageTask = stage->GetTaskQueue()->Async<Surface>(std::move(execute), std::move(callback));
    } catch (const std::exception& e) {
        LOG_ERROR(e);
        this->SetState(ResourceStateError, true);

        return;
    }

    LOG_INFO("%s: %s", ResourceStateToString(ResourceStateLoading), this->GetId());
    this->SetState(ResourceStateLoading, true);
}

static bool HasSvgHeader(const CFile& file) {
    uint8_t header[32];

    auto count{ file.Read(header, sizeof(header)) };

    file.Reset();

    for (std::size_t i = 0; i < count; i++) {
        if (header[i] == '<') {
            return true;
        } else if (!std::isspace(header[i])) {
            break;
        }
    }

    return false;
}

static Surface DecodeImage(const ImageUri& uri, const std::vector<std::string>& extensions,
             const std::vector<std::string>& resourcePath, const PixelFormat textureFormat) {
    Surface image;
    const auto& uriOrFilename{ uri.GetUri() };

    // TODO: support base64 encoded svgs
    // TODO: support url encoding

    if (IsDataUri(uriOrFilename)) {
        if (!IsSvgDataUri(uriOrFilename)) {
            throw std::runtime_error(Format("Invalid image data uri: %s", uriOrFilename));
        }

        const auto dataString{ GetSvgUriData(uriOrFilename) };
        const auto svg{ nsvgParse(const_cast<char *>(dataString.data()), "px", 96) };

        image = DecodeImageSvg(svg, uriOrFilename, uri.GetWidth(), uri.GetHeight());
    } else {
        const std::string filename {
            IsResourceUri(uriOrFilename) ?
                FindFile(GetResourceUriPath(uriOrFilename), extensions, resourcePath)
                    : FindFile(uriOrFilename, extensions)
        };
        const auto file{ CFile::Open(filename) };

        if (HasSvgHeader(file)) {
            file.Reset();

            image = DecodeImageSvg(
                nsvgParseFromFilePtr(static_cast<FILE*>(file), "px", 96),
                uriOrFilename,
                uri.GetWidth(),
                uri.GetHeight());
        } else {
            int32_t components{};
            int32_t width{};
            int32_t height{};
            const auto data{ stbi_load_from_file(static_cast<FILE*>(file), &width, &height, &components, 4) };

            if (data) {
                image = {
                    std::shared_ptr<uint8_t>(data, [] (uint8_t* p) { stbi_image_free(p); }),
                    width,
                    height,
                    width * 4,
                    rawImagePixelFormat
                };
            } else {
                throw std::runtime_error(Format("Failed to decode image: %s", filename));
            }
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
        throw std::runtime_error(Format("Failed to parse svg image: %s", uri));
    }

    const auto svgWidth{ static_cast<int32_t>(svg->width) };
    const auto svgHeight{ static_cast<int32_t>(svg->height) };

    if (svgWidth < 0 || svgHeight < 0) {
        throw std::runtime_error(Format("Failed to parse svg image: %s", uri));
    }

    if ((svgWidth == 0 || svgHeight == 0) && (scaleWidth == 0 && scaleHeight == 0)) {
        throw std::runtime_error(Format("No dimensions available for svg image: %s", uri));
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
        throw std::runtime_error(Format("Failed to create rasterizer for svg image: %s", uri));
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

    return { data, width, height, pitch, rawImagePixelFormat };
}

static
float ScaleFactor(const int source, const int dest) noexcept {
    return 1.f + ((dest - source) / static_cast<float>(source));
}

} // namespace ls
