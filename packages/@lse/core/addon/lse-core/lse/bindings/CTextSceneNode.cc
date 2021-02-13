/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CoreClasses.h"

#include <napix.h>
#include <lse/TextSceneNode.h>
#include <lse/bindings/CSceneNodeConstructor.h>

using napix::js_class::define;
using napix::descriptor::instance_accessor;

namespace lse {
namespace bindings {

static napi_value GetText(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, napix::unwrap_this_as<TextSceneNode>(env, info)->GetText());
}

static napi_value SetText(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  std::string text;

  if (napix::is_string(env, ci[0])) {
    text = napix::as_string_utf8(env, ci[0]);
  }

  ci.unwrap_this_as<TextSceneNode>(env)->SetText(std::move(text));

  return {};
}

napi_value CTextSceneNode::CreateClass(napi_env env) noexcept {
  auto props{ CSceneNode::GetClassProperties(env) };

  props.emplace_back(instance_accessor("text", &GetText, &SetText));

  return define(env, NAME, &CSceneNodeConstructor<TextSceneNode>, props.size(), props.data());
}

} // namespace bindings
} // namespace lse
