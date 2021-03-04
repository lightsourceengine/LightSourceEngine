/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <node_api.h>
#include <lse/Style.h>

namespace lse {
namespace bindings {

napi_value BoxColor(napi_env env, const std17::optional<color_t>& color) noexcept;
std17::optional<color_t> UnboxColor(napi_env env, napi_value value) noexcept;

napi_value BoxStyleValue(napi_env env, const StyleValue& value) noexcept;
std17::optional<StyleValue> UnboxStyleValue(napi_env env, napi_value value) noexcept;

napi_value BoxStyleTransformSpec(napi_env env, const std::vector<StyleTransformSpec>& transform) noexcept;
std::vector<StyleTransformSpec> UnboxStyleTransformSpec(napi_env env, napi_value value) noexcept;

napi_value BoxStyleFilterFunctionList(napi_env env, const std::vector<StyleFilterFunction>& list) noexcept;
std::vector<StyleFilterFunction> UnboxStyleFilterFunctionList(napi_env env, napi_value value) noexcept;

napi_value StyleGetter(napi_env env, Style* style, StyleProperty property);
void StyleSetter(napi_env env, Style* style, StyleProperty property, napi_value value);

} // namespace bindings
} // namespace lse

