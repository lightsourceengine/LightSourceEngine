/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/string-ext.h>

#include <algorithm>
#include <iterator>
#include <cctype>

namespace ls {

std::string& ToLowercase(std::string& str) noexcept {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

char* ToLowercase(char* str) noexcept {
    if (str) {
        size_t len = strlen(str);

        for (size_t i = 0; i < len; i++) {
            str[i] = static_cast<char>(::tolower(str[i]));
        }
    }

    return str;
}

} // namespace ls
