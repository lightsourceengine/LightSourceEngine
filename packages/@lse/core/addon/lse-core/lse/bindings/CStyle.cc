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

#include "CoreClasses.h"

#include <napix.h>
#include <lse/bindings/CStyleUtil.h>
#include <lse/Style.h>
#include <array>

using napix::unwrap_this_as;
using napix::js_class::define;
using napix::descriptor::instance_accessor;
using napix::descriptor::instance_method;

namespace lse {
namespace bindings {

static napi_value Constructor(napi_env env, napi_callback_info info) noexcept {
  return napix::js_class::constructor_helper(
      env,
      info,
      [](napi_env env, napi_callback_info info) -> void* {
        return new Style();
      },
      [](napi_env env, void* data, void* hint) {
        static_cast<Style*>(data)->Unref();
      });
}

static napi_value Get(napi_env env, napi_callback_info info) {
  auto style{napix::unwrap_this_as<Style>(env, info)};
  auto prop{reinterpret_cast<intptr_t>(napix::get_data_raw(env, info))};

  if (IsEnum<StyleProperty>(prop)) {
    return StyleGetter(env, style, static_cast<StyleProperty>(prop));
  }

  return {};
}

static napi_value Set(napi_env env, napi_callback_info info) {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto style{ci.unwrap_this_as<Style>(env)};

  NAPIX_EXPECT_FALSE(env, style->IsLocked(), "Style instance is immutable.", {});

  auto prop{reinterpret_cast<intptr_t>(napix::get_data_raw(env, info))};

  if (IsEnum<StyleProperty>(prop)) {
    StyleSetter(env, style, static_cast<StyleProperty>(prop), ci[0]);
  }

  return {};
}

napi_value CStyle::CreateClass(napi_env env) {
  std::array<napi_property_descriptor, Count<StyleProperty>()> props{};
  auto attributes{ static_cast<napi_property_attributes>(napi_enumerable | napi_writable) };

  for (size_t i = 0; i < props.size(); i++) {
    props[i] = instance_accessor(
        StylePropertyToString(static_cast<StyleProperty>(i)),
        &Get,
        &Set,
        i,
        attributes);
  }

  return define(env, NAME, Constructor, props.size(), props.data());
}

} // namespace bindings
} // namespace lse
