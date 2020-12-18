/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/System.h>
#include <cstdlib>

namespace ls {

bool HasEnv(const char* name) noexcept {
    return *GetEnv(name) != '\0';
}

const char* GetEnv(const char* name) noexcept {
    auto value = name ? std::getenv(name) : nullptr;

    if (value == nullptr || *value == '\0') {
        return "";
    }

    return value;
}

const char* GetEnvOrDefault(const char* name, const char* defaultValue) noexcept {
    return HasEnv(name) ? GetEnv(name) : defaultValue;
}

} // namespace ls
