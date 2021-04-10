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
