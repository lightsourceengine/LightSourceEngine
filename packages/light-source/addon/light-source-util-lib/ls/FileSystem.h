/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <vector>
#include <string>
#include <cstdio>
#include <memory>
#include <fs.h>

namespace ls {

/**
 * FILE handle that closes it's file pointer when the handle's destructor is run. Handy for controlling and
 * scoping file access.
 */
typedef std::unique_ptr<FILE, decltype(&fclose)> FileHandle;

/**
 * Checks if the basename of filename has a file extension.
 *
 * @returns "image.jpg", "path/image.jpg", "p.x" => true, "image" => false
 */
bool HasExtension(const fs::path& filename) noexcept;

/**
 * Searches for a file on disk. If filename does not exist, each file extension in extensions is appended to filename
 * and checked. App developers can specify a basename of an image and this can be used to search for an svg, gif, etc.
 *
 * @param filename The filename or basename to search on.
 * @param extensions List of extension strings in '.ext' format.
 */
fs::path FindFile(const fs::path& filename, const std::vector<std::string>& extensions);

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
fs::path FindFile(const fs::path& filename, const std::vector<std::string>& extensions,
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
fs::path GetResourceUriPath(const std::string& uri);

/**
 * Gets the data portion of a SVG uri.
 *
 * @param uri SVG uri that has been verified with IsSvgDataUri().
 */
std::string GetSvgUriData(const std::string& uri);

} // namespace ls
