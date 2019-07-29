/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <vector>
#include <string>

namespace ls {

std::vector<uint8_t> ReadBytes(const std::string filename);
bool HasExtension(const std::string& filename);
bool FileExists(const std::string& filename);
std::string Join(const std::string& path, const std::string& filename);
std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions);
std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions,
        const std::vector<std::string>& resourcePaths);

} // namespace ls
