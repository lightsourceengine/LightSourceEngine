/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageResource.h"
#include "FileSystem.h"
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <stb_image.h>
#include <fmt/format.h>

namespace ls {

using NSVGimageHandle = std::unique_ptr<NSVGimage, decltype(&nsvgDelete)>;
using NSVGrasterizerHandle = std::unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)>;

inline float ScaleFactor(const int source, const int dest);

ImageResource::ImageResource(Napi::Env env, const std::string& id) : Resource(env, id) {
    this->uri = id;
}

uint32_t ImageResource::GetTexture(Renderer* renderer) {
    this->data.reset();
    return 0;
}

void ImageResource::Load(Renderer* renderer,
        const std::vector<std::string>& extensions, const std::vector<std::string>& resourcePath) {
    auto initialState{ ResourceStateLoading };

    this->AddRef();

    try {
        this->work = std::make_unique<AsyncWork>(
            this->env,
            this->id,
            [=](Napi::Env env) {
                this->LoadImage(this->uri, extensions, resourcePath);
            },
            [this](Napi::Env env, napi_status status, const std::string& message) {
                // TODO: same code as FontResource
                if (this->resourceState != ResourceStateLoading) {
                    fmt::println("Warning: AsyncWork returned to a resource({}) in an unexpected state.",
                        this->GetId());
                    return;
                }

                this->RemoveRef();
                this->work.reset();

                if (this->GetRefCount() > 0) {
                    ResourceState nextState;

                    if (status != napi_ok) {
                        nextState = ResourceStateError;
                    } else {
                        nextState = ResourceStateReady;
                    }

                    this->SetStateAndNotifyListeners(nextState);
                }
            });
    } catch (std::exception& e) {
        this->RemoveRef();
        initialState = ResourceStateError;
    }

    this->SetStateAndNotifyListeners(initialState);
}

void ImageResource::LoadImage(const std::string& uriOrFilename, const std::vector<std::string>& extensions,
             const std::vector<std::string>& resourcePath) {
    if (IsDataUri(uriOrFilename)) {
        // TODO: support base64 encoded svgs

        if (IsSvgDataUri(uriOrFilename)) {
            auto data{ GetSvgUriData(uriOrFilename) };

            // TODO: should url decode here
            // TODO: get render width/height from uri params

            LoadImageFromSvg(nsvgParse(const_cast<char *>(data.c_str()), "px", 96), 0, 0);
        } else {
            throw std::runtime_error("invalid data uri");
        }
    } else {
        std::string filename;

        if (IsResourceUri(uriOrFilename)) {
            filename = FindFile(GetResourceUriPath(uriOrFilename), extensions, resourcePath);
        } else {
            filename = FindFile(uriOrFilename, extensions);
        }

        FileHandle file(fopen(filename.c_str(), "rb"), fclose);

        if (!file) {
            throw std::runtime_error("Could not open file,");
        }

        int32_t components{};
        int32_t width{};
        int32_t height{};

        auto data{ stbi_load_from_file(file.get(), &width, &height, &components, 4) };

        if (data) {
            this->width = width;
            this->height = height;
            this->data = std::shared_ptr<uint8_t>(data, [] (uint8_t* p) { stbi_image_free(p); });
            this->format = PixelFormatRGBA;
        } else {
            fseek(file.get(), 0, SEEK_SET);
            // TODO: get renderWidth / renderHeight from uri params
            LoadImageFromSvg(nsvgParseFromFilePtr(file.get(), "px", 96), 0, 0);
        }
    }
}

void ImageResource::LoadImageFromSvg(
        NSVGimage* svgImage, const int32_t scaleWidth, const int32_t scaleHeight) {
    NSVGimageHandle svg(svgImage, nsvgDelete);

    // XXX: nsvgParse* methods do not check if parsing failed. NSVGImage can be left in a partially filled out state,
    // resulting in a bad render. Negative width and height are an indication that parsing failed, but that does not
    // cover all invalid XML use cases.

    if (svg == nullptr) {
        throw std::runtime_error("Failed to parse image.");
    }

    auto svgWidth{ static_cast<int32_t>(svg->width) };
    auto svgHeight{ static_cast<int32_t>(svg->height) };

    if (svgWidth < 0 || svgHeight < 0) {
        throw std::runtime_error("Failed to parse image.");
    }

    if ((svgWidth == 0 || svgHeight == 0) && (scaleWidth == 0 && scaleHeight == 0)) {
        throw std::runtime_error("SVG contains no dimensions.");
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
        throw std::runtime_error("Failed to create rasterizer SVG image.");
    }

    std::shared_ptr<uint8_t> data(new uint8_t [width * height * 4], [](uint8_t* p){ delete [] p; });
    // TODO: set format

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

    this->width = width;
    this->height = height;
    this->data = data;
    this->format = PixelFormatRGBA;
}

inline float ScaleFactor(const int source, const int dest) {
    return 1.f + ((dest - source) / static_cast<float>(source));
}

} // namespace ls
