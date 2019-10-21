/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <vector>
#include <string>
#include <cstdio>

namespace ls {

// Manages a FILE* based file.
class CFile {
 private:
    CFile(FILE* file) noexcept; // NOLINT

 public:
    ~CFile() noexcept;

    static CFile Open(const std::string& filename, const char* access = "rb");
    std::size_t GetSize() const noexcept;
    std::size_t Read(uint8_t* buffer, const std::size_t byteCount) const;
    void Reset() const noexcept;
    explicit operator FILE*() const { return this->file; }

 private:
    FILE* file;
};

/**
 * Searches for a file on disk. If filename does not exist, each file extension in extensions is appended to filename
 * and checked. App developers can specify a basename of an image and this can be used to search for an svg, gif, etc.
 *
 * @param filename The filename or basename to search on.
 * @param extensions List of extension strings in '.ext' format.
 */
std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions);

/**
 * Searches for a file on disk.
 *
 * Resource paths are just a list of directories to search in. In each directory, if filename does not exist, each
 * file extension in extensions is appended to filename and checked. App developers can specify a basename of an image
 * and this can be used to search for an svg, gif, etc.
 *
 * @param filename The filename or basename to search on. This should be a relative filename.
 * @param extensions List of extension strings in '.ext' format.
 * @param resourcePaths List of resource directories to search in.
 */
std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions,
        const std::vector<std::string>& resourcePaths);

/**
 * Checks if the specified uri is a data uri that light source can parse.
 */
bool IsDataUri(const std::string& uri) noexcept;

/**
 * Checks if the specified file uri points to the resource domain (file://resource/). When the resource domain is
 * used, resource manager paths will be searched.
 */
bool IsResourceUri(const std::string& uri) noexcept;

/**
 * Checks if the specified uri is a compatible SVG uri.
 */
bool IsSvgDataUri(const std::string& uri) noexcept;

/**
 * Gets the relative path from a resource file uri.
 *
 * @param uri Resource file uri that has been verified with IsResourceUri().
 */
std::string GetResourceUriPath(const std::string& uri);

/**
 * Gets the data portion of a SVG uri.
 *
 * @param uri SVG uri that has been verified with IsSvgDataUri().
 */
std::string GetSvgUriData(const std::string& uri);

// Expose to the test environment
namespace internal {

bool HasExtension(const std::string& filename) noexcept;

} // namespace internal

} // namespace ls
