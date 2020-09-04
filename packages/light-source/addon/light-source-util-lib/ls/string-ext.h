/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <string>
#include <std17/string_view>

namespace ls {

// Simple ascii std::string conversion to lowercase, no intl or encoding considered.
std::string& ToLowercase(std::string& str) noexcept;

// Simple ascii cstring conversion to lowercase, no intl or encoding considered. If str is null, null is returned.
char* ToLowercase(char* str) noexcept;

} // namespace ls
