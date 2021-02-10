/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
