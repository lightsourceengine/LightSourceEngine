/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/Style.h>

namespace lse {
namespace bindings {

/**
 * Bindings for a javascript StyleTransformSpec object.
 */
class JSStyleTransformSpec {
 public:
    static Napi::Function GetClass(const Napi::Env& env);
    static Napi::Object New(const Napi::Env& env, const StyleTransformSpec& value);
    static Napi::Array New(const Napi::Env& env, const std::vector<StyleTransformSpec>& value);
    static Napi::Value Undefined(const Napi::Env& env);
    static std::vector<StyleTransformSpec> ToStyleTransformSpecList(const Napi::Value& value);
};

} // namespace bindings
} // namespace lse
