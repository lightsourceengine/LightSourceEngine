/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi.h>

namespace Napi {

/**
 * Helper wrapper for passing property names to the SafeObjectWrap binding methods.
 */
struct PropertyName {
    const char* utf8Name;
    napi_value name;

    PropertyName(const char* utf8Name) noexcept : utf8Name(utf8Name), name(nullptr) {}
    PropertyName(const Symbol& value) noexcept : utf8Name(nullptr), name(value) {}
};

} // namespace Napi
