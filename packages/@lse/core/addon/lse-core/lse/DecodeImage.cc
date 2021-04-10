/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include <lse/DecodeImage.h>

#include <lse/string-ext.h>
#include <stb_image.h>
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <array>
#include <base64.h>

using NSVGimagePtr = std::unique_ptr<NSVGimage, decltype(&nsvgDelete)>;
using NSVGrasterizerPtr = std::unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)>;
using ByteBufferPtr = std::unique_ptr<uint8_t, std::function<void(uint8_t*)>>;

static constexpr int32_t kNumChannels = 4;
static constexpr auto kDataUriSvgUtf8 = "data:image/svg+xml;utf8,";
static constexpr auto kDataUriSvgBase64 = "data:image/svg+xml;base64,";
static constexpr auto kDataUriImageBase64 = "data:image/image;base64,";
static const auto kDataUriSvgUtf8Len = strlen(kDataUriSvgUtf8);
static const auto kDataUriSvgBase64Len = strlen(kDataUriSvgBase64);
static const auto kDataUriImageBase64Len = strlen(kDataUriImageBase64);

// TODO: this list should be user configurable.
// List of image extensions to search when path contains the '.*' extension.
static constexpr std::array<const char*, 5> kImageExtensions{{
  ".png",
  ".jpg",
  ".jpeg",
  ".gif",
  ".svg"
}};

namespace lse {

bool ScaleSvg(
    NSVGimagePtr& svg, int32_t scaleWidth, int32_t scaleHeight,
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
    const auto ScaleFactor{
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

void DeleteSvgBytes(uint8_t* p) noexcept {
  // Allocated in LoadSvg() during rasterization.
  delete[] p;
}

void DeleteStbBytes(uint8_t* p) noexcept {
  stbi_image_free(p);
}

uint8_t* LoadSvg(NSVGimagePtr& svg, const char* name, int32_t scaleWidth, int32_t scaleHeight,
    int32_t* width, int32_t* height) {
  float scaleX{};
  float scaleY{};
  int32_t renderWidth{};
  int32_t renderHeight{};

  if (!svg) {
    throw std::runtime_error(Format("Error parsing SVG: %s", name));
  }

  if (!ScaleSvg(svg, scaleWidth, scaleHeight, &scaleX, &scaleY, &renderWidth, &renderHeight)) {
    throw std::runtime_error(Format("Error scaling SVG: %s", name));
  }

  NSVGrasterizerPtr rasterizer(nsvgCreateRasterizer(), nsvgDeleteRasterizer);

  if (!rasterizer) {
    throw std::runtime_error(Format("Error allocating rasterizer SVG: %s", name));
  }

  const auto stride{ renderWidth * kNumChannels };
  ByteBufferPtr data(new uint8_t[renderHeight * stride], [](uint8_t* p) { delete[] p; });

  nsvgRasterizeFull(
      rasterizer.get(), svg.get(), 0, 0, scaleX, scaleY, data.get(), renderWidth, renderHeight, stride);

  *width = renderWidth;
  *height = renderHeight;

  return data.release();
}

uint8_t* LoadSvgFromFile(
    const std17::filesystem::path& path, int32_t scaleWidth, int32_t scaleHeight,
    int32_t* width, int32_t* height) {
  if (!std17::filesystem::exists(path)) {
    throw std::runtime_error(Format("Error SVG file not found: %s", path.c_str()));
  }

  NSVGimagePtr svg(nsvgParseFromFile(path.c_str(), "px", 96), nsvgDelete);

  return LoadSvg(svg, path.c_str(), scaleWidth, scaleHeight, width, height);
}

uint8_t* LoadSvgFromMemory(char* xml, int32_t scaleWidth, int32_t scaleHeight, int32_t* width, int32_t* height) {
  NSVGimagePtr svg(nsvgParse(xml, "px", 96), nsvgDelete);

  // TODO: use data uri
  return LoadSvg(svg, xml, scaleWidth, scaleHeight, width, height);
}

ImageBytes DecodeImageFromFile(const std17::filesystem::path& path, int32_t resizeWidth, int32_t resizeHeight) {
  uint8_t* bytes;
  ImageBytes::Deleter deleter;
  int32_t width{};
  int32_t height{};

  if (EndsWith(path.c_str(), ".*")) {
    // Handle the '.*' extension search recursively.
    auto pathCopy{ path };
    std::error_code errorCode;

    for (auto ext : kImageExtensions) {
      pathCopy.replace_extension(ext);

      if (!std17::filesystem::exists(pathCopy, errorCode)) {
        continue;
      }

      try {
        return DecodeImageFromFile(pathCopy.replace_extension(ext), resizeWidth, resizeHeight);
      } catch (std::exception&) {
        // continue search
      }
    }

    throw std::runtime_error(Format("No image file found for %s", path.c_str()));
  } else if (EndsWith(path.c_str(), ".svg")) {
    // Special handling for SVG images.
    bytes = LoadSvgFromFile(path, resizeWidth, resizeHeight, &width, &height);
    deleter = &DeleteSvgBytes;
  } else {
    int32_t components{};

    bytes = stbi_load(path.c_str(), &width, &height, &components, kNumChannels);

    if (!bytes) {
      throw std::runtime_error(Format("Failed to load image %s", path.c_str()));
    }

    deleter = &DeleteStbBytes;
  }

  return { bytes, deleter, width, height, width * 4 };
}

std::shared_ptr<unsigned char> DecodeBase64DataUri(const char* rawBase64, size_t* decodedLen = nullptr) {
  size_t temp{};
  auto encoded = reinterpret_cast<const unsigned char*>(rawBase64);
  auto decoded = std::shared_ptr<unsigned char>(
      base64_decode(encoded, strlen(rawBase64), decodedLen ? decodedLen : &temp),
      [](void* p) { free(p); });

  if (!decoded) {
    throw std::runtime_error("Invalid base64 data uri");
  }

  return decoded;
}

ImageBytes DecodeImageFromDataUri(const std::string& uri, int32_t resizeWidth, int32_t resizeHeight) {
  uint8_t* bytes{};
  ImageBytes::Deleter deleter;
  int32_t width{};
  int32_t height{};

  if (StartsWith(uri, kDataUriSvgUtf8) && uri.size() > kDataUriSvgUtf8Len) {
    auto xml{ uri.substr(kDataUriSvgUtf8Len) };

    bytes = LoadSvgFromMemory(&xml[0], resizeWidth, resizeHeight, &width, &height);
    deleter = &DeleteSvgBytes;
  } else if (StartsWith(uri, kDataUriSvgBase64) && uri.size() > kDataUriSvgBase64Len) {
    auto decoded{ DecodeBase64DataUri(&uri[kDataUriSvgBase64Len]) };

    bytes = LoadSvgFromMemory(reinterpret_cast<char*>(decoded.get()), resizeWidth, resizeHeight, &width, &height);
    deleter = &DeleteSvgBytes;
  } else if (StartsWith(uri, kDataUriImageBase64) && uri.size() > kDataUriImageBase64Len) {
    // TODO: svg is not supported through this path
    size_t decodedLen{};
    auto decoded{ DecodeBase64DataUri(&uri[kDataUriImageBase64Len], &decodedLen) };
    int32_t components{};

    if (decoded) {
      bytes = stbi_load_from_memory(decoded.get(), decodedLen, &width, &height, &components, kNumChannels);
    }

    if (!bytes) {
      throw std::runtime_error(Format("Failed to load image %s", uri));
    }

    deleter = &DeleteStbBytes;
  } else {
    throw std::runtime_error(Format("Unsupported data uri: %s", uri));
  }

  return { bytes, deleter, width, height, width * 4 };
}

} // namespace lse
