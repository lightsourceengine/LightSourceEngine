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

namespace ls {

typedef std::unique_ptr<FILE, decltype(&fclose)> FileHandle;

bool HasExtension(const std::string& filename);
bool FileExists(const std::string& filename);
std::string Join(const std::string& path, const std::string& filename);
std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions);
std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions,
        const std::vector<std::string>& resourcePaths);
bool IsDataUri(const std::string& uri);
bool IsResourceUri(const std::string& uri);
bool IsSvgDataUri(const std::string& uri);
std::string GetResourceUriPath(const std::string& resourceUri);
std::string GetSvgUriData(const std::string& svgUri);

} // namespace ls
