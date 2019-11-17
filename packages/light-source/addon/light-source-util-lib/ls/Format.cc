/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Format.h"

namespace ls {
namespace internal {

const char* FormatArg(const bool& value) noexcept {
    return value ? "true" : "false";
}

const char* FormatArg(const std::string& value) noexcept {
    return value.c_str();
}

const char* FormatArg(std::nullptr_t) noexcept {
    return "null";
}

const char* FormatArg(const char* value) noexcept {
    return value ? value : "null";
}

const char* FormatArg(const std::exception& e) noexcept {
    return e.what();
}

} // namespace internal
} // namespace ls
