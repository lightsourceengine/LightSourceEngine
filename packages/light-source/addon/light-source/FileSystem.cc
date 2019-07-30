/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FileSystem.h"
#include <sys/stat.h>

constexpr auto DATA_URI_PREFIX = "data:";
const auto DATA_URI_PREFIX_LEN = std::strlen(DATA_URI_PREFIX);
constexpr auto RESOURCE_URI_PREFIX = "file://resource/";
const auto RESOURCE_URI_PREFIX_LEN = std::strlen(RESOURCE_URI_PREFIX);
constexpr auto SVG_DATA_URI_PREFIX = "data:image/svg+xml,";
const auto SVG_DATA_URI_PREFIX_LEN = std::strlen(SVG_DATA_URI_PREFIX);
constexpr auto PATH_SEPARATOR =
#ifdef _WIN32
    '\\';
#else
    '/';
#endif

namespace ls {

std::vector<uint8_t> ReadBytes(const std::string filename) {
    std::vector<uint8_t> buffer;
    FileHandle file(fopen(filename.c_str(), "rb"), fclose);

    if (!file) {
        throw std::runtime_error("Failed to open file.");
    }

    fseek(file.get(), 0, SEEK_END);
    auto size{ static_cast<size_t>(ftell(file.get())) };
    fseek(file.get(), 0, SEEK_SET);

    buffer.reserve(size);

    if (fread(&buffer[0], 1, size, file.get()) != size) {
        throw std::runtime_error("Failed to read file.");
    }

    return buffer;
}


bool HasExtension(const std::string& filename) {
    auto sepIndex = filename.find_last_of("/\\");

    if (sepIndex == std::string::npos) {
        sepIndex = 0;
    }

    return filename.find('.', sepIndex + 1) != std::string::npos;
}

bool FileExists(const std::string& filename) {
  struct stat buffer;
  return (stat(filename.c_str(), &buffer) == 0);
}

std::string Join(const std::string& path, const std::string& filename) {
    return path + PATH_SEPARATOR + filename;
}

std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions) {
    if (FileExists(filename)) {
        return filename;
    }

    if (!HasExtension(filename)) {
        std::string filenameWithExtension;

        for (auto& ext : extensions) {
            filenameWithExtension = filename + ext;

            if (FileExists(filenameWithExtension)) {
                return filenameWithExtension;
            }
        }
    }

    throw std::runtime_error("File not found: " + filename);
}

std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions,
        const std::vector<std::string>& resourcePaths) {
    for (auto& resourcePath : resourcePaths) {
        try {
            return FindFile(Join(resourcePath, filename), extensions);
        } catch (std::exception& e) {
            // continue
        }
    }

    throw std::runtime_error("File not found: " + filename);
}

bool IsDataUri(const std::string& uri) {
    return strncmp(uri.c_str(), DATA_URI_PREFIX, DATA_URI_PREFIX_LEN) == 0;
}

bool IsResourceUri(const std::string& uri) {
    return strncmp(uri.c_str(), RESOURCE_URI_PREFIX, RESOURCE_URI_PREFIX_LEN) == 0;
}

bool IsSvgDataUri(const std::string& uri) {
    return strncmp(uri.c_str(), SVG_DATA_URI_PREFIX, SVG_DATA_URI_PREFIX_LEN) == 0
        && uri.size() > SVG_DATA_URI_PREFIX_LEN;
}

std::string GetResourceUriPath(const std::string& resourceUri) {
    return resourceUri.substr(RESOURCE_URI_PREFIX_LEN);
}

std::string GetSvgUriData(const std::string& svgUri) {
    return svgUri.substr(SVG_DATA_URI_PREFIX_LEN);
}

} // namespace ls
