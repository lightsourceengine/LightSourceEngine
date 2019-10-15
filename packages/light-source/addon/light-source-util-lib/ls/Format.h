/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <string>
#include <memory>

namespace ls {

namespace internal {

template<typename T>
const T& FormatArg(const T& value) noexcept {
    return value;
}
const char* FormatArg(const bool& value) noexcept;
const char* FormatArg(const std::string& value) noexcept;
const char* FormatArg(const char* value) noexcept;
const char* FormatArg(std::nullptr_t) noexcept;

} // namespace internal

/**
 * Format an std::string using printf semantics.
 *
 * In addition to full printf support, Format can handle std::string, null char*, nullptr and bool.
 */
template<typename... Args>
std::string Format(const char* format, const Args&... args) {
    char t[33];

    // Try writing to the stack.
    auto size{ snprintf(&t[0], sizeof(t), format, internal::FormatArg(args)... ) };

    if (size <= 0) {
        return "";
    } else if (size < static_cast<int32_t>(sizeof(t)) - 1) {
        return std::string(t);
    }

    // If stack allocation is insufficient, create enough room on the heap for the resulting string.
    auto buffer{ std::make_unique<char[]>(size + 1) };

    size = snprintf(buffer.get(), size + 1, format, internal::FormatArg(args)...);

    if (size <= 0) {
        return "";
    }

    return std::string(buffer.get(), size);
}

} // namespace ls
