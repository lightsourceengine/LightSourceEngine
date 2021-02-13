/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CoreClasses.h"

#include <napix.h>
#include <lse/ImageSceneNode.h>
#include <lse/bindings/CSceneNodeConstructor.h>

using napix::js_class::define;
using napix::descriptor::instance_accessor;

namespace lse {
namespace bindings {

static napi_value GetSource(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, napix::unwrap_this_as<ImageSceneNode>(env, info)->GetSource());
}

static napi_value SetSource(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<ImageSceneNode>(env)};
  std::string src;

  if (napix::is_string(env, ci[0])) {
    src = napix::as_string_utf8(env, ci[0]);
  }

  node->SetSource(env, std::move(src));

  return {};
}

static napi_value GetOnLoad(napi_env env, napi_callback_info info) noexcept {
  return napix::unwrap_this_as<ImageSceneNode>(env, info)->GetOnLoadCallback(env);
}

static napi_value SetOnLoad(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<ImageSceneNode>(env)};

  node->SetOnLoadCallback(env, ci[0]);

  return {};
}

static napi_value GetOnError(napi_env env, napi_callback_info info) noexcept {
  return napix::unwrap_this_as<ImageSceneNode>(env, info)->GetOnErrorCallback(env);
}

static napi_value SetOnError(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<ImageSceneNode>(env)};

  node->SetOnErrorCallback(env, ci[0]);

  return {};
}

napi_value CImageSceneNode::CreateClass(napi_env env) noexcept {
  auto props{ CSceneNode::GetClassProperties(env) };

  props.emplace_back(instance_accessor("src", &GetSource, &SetSource));
  props.emplace_back(instance_accessor("onLoad", &GetOnLoad, &SetOnLoad));
  props.emplace_back(instance_accessor("onError", &GetOnError, &SetOnError));

  return define(env, NAME, &CSceneNodeConstructor<ImageSceneNode>, props.size(), props.data());
}

} // namespace bindings
} // namespace lse
