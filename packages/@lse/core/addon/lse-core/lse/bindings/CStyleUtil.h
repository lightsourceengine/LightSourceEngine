/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <node_api.h>
#include <lse/Style.h>

namespace lse {
namespace bindings {

napi_value BoxColor(napi_env env, const std::optional<color_t>& color) noexcept;
std::optional<color_t> UnboxColor(napi_env env, napi_value value) noexcept;

napi_value BoxStyleValue(napi_env env, const StyleValue& value) noexcept;
std::optional<StyleValue> UnboxStyleValue(napi_env env, napi_value value) noexcept;

napi_value BoxStyleTransformSpec(napi_env env, const std::vector<StyleTransformSpec>& transform) noexcept;
std::vector<StyleTransformSpec> UnboxStyleTransformSpec(napi_env env, napi_value value) noexcept;

napi_value BoxStyleFilterFunctionList(napi_env env, const std::vector<StyleFilterFunction>& list) noexcept;
std::vector<StyleFilterFunction> UnboxStyleFilterFunctionList(napi_env env, napi_value value) noexcept;

napi_value StyleGetter(napi_env env, Style* style, StyleProperty property);
void StyleSetter(napi_env env, Style* style, StyleProperty property, napi_value value);

} // namespace bindings
} // namespace lse

