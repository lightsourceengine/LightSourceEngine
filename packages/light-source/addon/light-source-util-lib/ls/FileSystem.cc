/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FileSystem.h"
#include <cstring>

constexpr auto DATA_URI_PREFIX = "data:";
const auto DATA_URI_PREFIX_LEN = std::strlen(DATA_URI_PREFIX);
constexpr auto RESOURCE_URI_PREFIX = "file://resource/";
const auto RESOURCE_URI_PREFIX_LEN = std::strlen(RESOURCE_URI_PREFIX);
constexpr auto SVG_DATA_URI_PREFIX = "data:image/svg+xml,";
const auto SVG_DATA_URI_PREFIX_LEN = std::strlen(SVG_DATA_URI_PREFIX);

namespace ls {

bool HasExtension(const fs::path& filename) noexcept {
    auto sepIndex{ filename.native().find_last_of(fs::path::preferred_separator) };

    if (sepIndex == std::string::npos) {
        sepIndex = 0;
    }

    return filename.native().find('.', sepIndex + 1) != std::string::npos;
}

fs::path FindFile(const fs::path& filename, const std::vector<std::string>& extensions) {
    if (fs::exists(filename)) {
        return filename;
    }

    if (!HasExtension(filename)) {
        for (const auto& ext : extensions) {
            fs::path filenameWithExtension{ filename };

            filenameWithExtension += ext;

            if (fs::exists(filenameWithExtension)) {
                return filenameWithExtension;
            }
        }
    }

    throw std::runtime_error("File not found: " + filename.native());
}

fs::path FindFile(const fs::path& filename, const std::vector<std::string>& extensions,
        const std::vector<std::string>& resourcePaths) {
    for (const auto& resourcePath : resourcePaths) {
        try {
            return FindFile(fs::path(resourcePath) / filename, extensions);
        } catch (const std::exception&) {
            // continue
        }
    }

    throw std::runtime_error("File not found: " + filename.native());
}

bool IsDataUri(const std::string& uri) noexcept {
    return strncmp(uri.c_str(), DATA_URI_PREFIX, DATA_URI_PREFIX_LEN) == 0;
}

bool IsResourceUri(const std::string& uri) noexcept {
    return strncmp(uri.c_str(), RESOURCE_URI_PREFIX, RESOURCE_URI_PREFIX_LEN) == 0;
}

bool IsSvgDataUri(const std::string& uri) noexcept {
    return strncmp(uri.c_str(), SVG_DATA_URI_PREFIX, SVG_DATA_URI_PREFIX_LEN) == 0
        && uri.size() > SVG_DATA_URI_PREFIX_LEN;
}

fs::path GetResourceUriPath(const std::string& resourceUri) {
    return { resourceUri.substr(RESOURCE_URI_PREFIX_LEN) };
}

std::string GetSvgUriData(const std::string& svgUri) {
    return svgUri.substr(SVG_DATA_URI_PREFIX_LEN);
}

} // namespace ls
