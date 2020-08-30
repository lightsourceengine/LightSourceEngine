/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace ls {
namespace bindings {

// returns: B::Cast(value)->GetNative() -> TRef
template<typename T, typename B>
std::shared_ptr<T> ToNative(const Napi::Value& value) noexcept {
    auto bindingInstance{ B::Cast(value) };
    return bindingInstance ? bindingInstance->GetNative() : nullptr;
}

} // namespace bindings
} // namespace ls
