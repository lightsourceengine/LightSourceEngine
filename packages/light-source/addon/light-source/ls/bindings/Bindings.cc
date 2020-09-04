/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/bindings/Bindings.h>

#include <ls/StyleEnums.h>
#include <ls/bindings/Convert.h>

namespace ls {
namespace bindings {

Napi::Value ParseColor(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };

    return BoxColor(env, UnboxColor(env, info[0]));
}

Napi::Object GetStyleProperties(Napi::Env env) {
    auto styleProperties{ Napi::Object::New(env) };

    for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
        styleProperties[ToString(static_cast<StyleProperty>(i))] = Napi::Number::New(env, i);
    }

    return styleProperties;
}

} // namespace bindings
} // namespace ls
