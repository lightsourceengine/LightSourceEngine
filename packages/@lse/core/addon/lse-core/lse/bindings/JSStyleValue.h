/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/Style.h>

namespace ls {
namespace bindings {

/**
 * Bindings for a javascript StyleValue object.
 */
class JSStyleValue {
 public:
    static Napi::Function GetClass(const Napi::Env& env);
    static Napi::Object New(const Napi::Env& env, const StyleValue& value);
    static Napi::Object Undefined(const Napi::Env& env);
    static StyleValue ToStyleValue(const Napi::Value& value);
};

} // namespace bindings
} // namespace ls
