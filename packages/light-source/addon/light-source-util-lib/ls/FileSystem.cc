/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FileSystem.h"
#include "Format.h"
#include <cstring>
#include <sys/stat.h>
#include <fs.h>

constexpr auto DATA_URI_PREFIX = "data:";
const auto DATA_URI_PREFIX_LEN = std::strlen(DATA_URI_PREFIX);
constexpr auto RESOURCE_URI_PREFIX = "file://resource/";
const auto RESOURCE_URI_PREFIX_LEN = std::strlen(RESOURCE_URI_PREFIX);
constexpr auto SVG_DATA_URI_PREFIX = "data:image/svg+xml,";
const auto SVG_DATA_URI_PREFIX_LEN = std::strlen(SVG_DATA_URI_PREFIX);

namespace ls {

static std::string FindFile(const fs::path& path, const std::vector<std::string>& extensions);

CFile::CFile(FILE* file) noexcept : file(file) {
}

CFile::~CFile() noexcept {
    fclose(this->file);
}

CFile CFile::Open(const std::string& filename, const char* access) {
    auto file{ fopen(filename.c_str(), access) };

    if (!file) {
        throw std::runtime_error(Format("Cannot open file %s", filename));
    }

    return { file };
}

std::size_t CFile::GetSize() const noexcept {
    const auto position{ ftell(this->file) };

    fseek(this->file, 0, SEEK_END);

    const auto size{ ftell(this->file) };

    fseek(this->file, position, SEEK_SET);

    return static_cast<std::size_t>(size);
}

std::size_t CFile::Read(uint8_t* buffer, const std::size_t byteCount) const {
    auto bytesRead{ fread(buffer, 1, byteCount, this->file) };
    auto result{ ferror(this->file) };

    if (result) {
        clearerr(this->file);
        throw std::runtime_error(Format("File read error: %i", result));
    }

    return bytesRead;
}

void CFile::Reset() const noexcept {
    fseek(this->file, 0, SEEK_SET);
}

static std::string FindFile(const fs::path& path, const std::vector<std::string>& extensions) {
    if (fs::exists(path)) {
        return path;
    }

    fs::path pathWithExtension;

    if (!internal::HasExtension(path.native())) {
        for (const auto& ext : extensions) {
            pathWithExtension = path;
            pathWithExtension += ext;

            if (exists(pathWithExtension)) {
                return pathWithExtension;
            }
        }
    }

    throw std::runtime_error(Format("File not found: %s", path.c_str()));
}

std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions) {
    return FindFile(fs::path(filename), extensions);
}

std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions,
        const std::vector<std::string>& resourcePaths) {
    fs::path path;

    for (const auto& resourcePath : resourcePaths) {
        try {
            path = resourcePath;
            path.append(filename);
            path.lexically_normal();

            return FindFile(path, extensions);
        } catch (const std::exception&) {
            // continue
        }
    }

    throw std::runtime_error("File not found: " + filename);
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

std::string GetResourceUriPath(const std::string& resourceUri) {
    fs::path path{resourceUri.substr(RESOURCE_URI_PREFIX_LEN)};

    path.lexically_normal();

    return path;
}

std::string GetSvgUriData(const std::string& svgUri) {
    return svgUri.substr(SVG_DATA_URI_PREFIX_LEN);
}

namespace internal {

bool HasExtension(const std::string& filename) noexcept {
    // Assumes filename is lexically_normal.
    auto sepIndex{ filename.find_last_of(fs::path::preferred_separator) };

    if (sepIndex == std::string::npos) {
        sepIndex = 0;
    }

    return filename.find('.', sepIndex + 1) != std::string::npos;
}

} // namespace internal

} // namespace ls
