/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/DecodeImage.h>

#include <ls/Format.h>
#include <stb_image.h>
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <array>

using NSVGimagePtr = std::unique_ptr<NSVGimage, decltype(&nsvgDelete)>;
using NSVGrasterizerPtr = std::unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)>;
using ByteBufferPtr = std::unique_ptr<uint8_t, std::function<void(uint8_t*)>>;
using std17::filesystem::detail::equals_simple_insensitive;

constexpr int32_t kNumChannels = 4;

// TODO: this list should be user configurable.
// List of image extensions to search when path contains the '.*' extension.
constexpr std::array<const char*, 5> kImageExtensions {{
    ".png",
    ".jpg",
    ".jpeg",
    ".gif",
    ".svg"
}};

namespace ls {

bool ScaleSvg(NSVGimagePtr& svg, int32_t scaleWidth, int32_t scaleHeight,
              float* scaleX, float* scaleY, int32_t* renderWidth, int32_t* renderHeight) noexcept {
    const auto svgWidth{ static_cast<int32_t>(svg->width) };
    const auto svgHeight{ static_cast<int32_t>(svg->height) };

    if (svgWidth < 0 || svgHeight < 0) {
        return false;
    }

    if ((svgWidth == 0 || svgHeight == 0) && (scaleWidth == 0 && scaleHeight == 0)) {
        return false;
    }

    if (scaleWidth > 0 && scaleHeight > 0) {
        const auto ScaleFactor {
            [](const int source, const int dest) noexcept {
              return 1.f + (static_cast<float>(dest - source) / static_cast<float>(source));
            }
        };

        *renderWidth = scaleWidth;
        *renderHeight = scaleHeight;
        *scaleX = ScaleFactor(svgWidth, scaleWidth);
        *scaleY = ScaleFactor(svgHeight, scaleHeight);
    } else {
        *renderWidth = svgWidth;
        *renderHeight = svgHeight;
        *scaleX = 1.0f;
        *scaleY = 1.0f;
    }

    return true;
}

uint8_t* LoadSvg(const std17::filesystem::path& path, int32_t scaleWidth, int32_t scaleHeight,
        int32_t* width, int32_t* height) {
    if (!std17::filesystem::exists(path)) {
        throw std::runtime_error(Format("Error SVG file not found: %s", path.c_str()));
    }

    NSVGimagePtr svg(nsvgParseFromFile(path.c_str(), "px", 96), nsvgDelete);

    if (!svg) {
        throw std::runtime_error(Format("Error parsing SVG: %s", path.c_str()));
    }

    float scaleX{};
    float scaleY{};
    int32_t renderWidth{};
    int32_t renderHeight{};

    if (!ScaleSvg(svg, scaleWidth, scaleHeight, &scaleX, &scaleY, &renderWidth, &renderHeight)) {
        throw std::runtime_error(Format("Error scaling SVG: %s", path.c_str()));
    }

    NSVGrasterizerPtr rasterizer(nsvgCreateRasterizer(), nsvgDeleteRasterizer);

    if (!rasterizer) {
        throw std::runtime_error(Format("Error allocating rasterizer SVG: %s", path.c_str()));
    }

    const auto stride{ renderWidth * kNumChannels };
    uint8_t* ptr;

    try {
        ptr = new uint8_t[renderHeight * stride];
    } catch (std::exception& e) {
        throw std::runtime_error(
            Format("Error allocating byte buffer SVG: %s %ix%i", path.c_str(), renderWidth, renderHeight));
    }

    ByteBufferPtr data(ptr, [](uint8_t* p){ delete [] p; });

    nsvgRasterizeFull(
        rasterizer.get(), svg.get(), 0, 0, scaleX, scaleY, data.get(), renderWidth, renderHeight, stride);

    *width = renderWidth;
    *height = renderHeight;

    return data.release();
}

ImageBytes DecodeImageFromFile(const std17::filesystem::path& path, int32_t resizeWidth, int32_t resizeHeight) {
    uint8_t* bytes;
    ImageBytes::Deleter deleter;
    int32_t width{};
    int32_t height{};

    if (equals_simple_insensitive(path.extension().c_str(), ".*")) {
        // Handle the '.*' extension search recursively.
        auto pathCopy{ path };
        std::error_code errorCode;

        for (auto ext : kImageExtensions) {
            pathCopy.replace_extension(ext);

            if (!std17::filesystem::exists(path, errorCode)) {
                continue;
            }

            try {
                return DecodeImageFromFile(pathCopy.replace_extension(ext), resizeWidth, resizeHeight);
            } catch (std::exception& e) {
                // continue search
            }
        }

        throw std::runtime_error(Format("No image file found for %s", path.c_str()));
    } else if (equals_simple_insensitive(path.extension().c_str(), ".svg")) {
        // Special handling for SVG images.
        bytes = LoadSvg(path.c_str(), resizeWidth, resizeHeight, &width, &height);
        deleter = [](uint8_t* p) noexcept { delete [] p; };
    } else {
        int32_t components{};

        bytes = stbi_load(path.c_str(), &width, &height, &components, kNumChannels);
        deleter = [](uint8_t* p) noexcept { stbi_image_free(p); };
    }

    return { bytes, deleter, width, height, width * 4 };
}

} // namespace ls
