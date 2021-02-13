/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CoreClasses.h"

#include <napix.h>
#include <lse/Habitat.h>

using napix::js_class::define;
using napix::descriptor::instance_method;

namespace lse {
namespace bindings {

static SceneNode* ToSceneNode(napi_env env, napi_value value) noexcept;

static napi_value BindStyle(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<SceneNode>(env)};

  if (!Habitat::InstanceOf(env, ci[0], Habitat::Class::CStyle)) {
    napix::throw_error(env, "expected a CStyle instance");
    return {};
  }

  auto style{napix::unwrap_as<Style>(env, ci[0])};

  if (!style) {
    napix::throw_error(env, "can't unwrap style");
    return {};
  }

  node->BindStyle(style);

  return ci[0];
}

static napi_value GetX(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, napix::unwrap_this_as<SceneNode>(env, info)->GetX());
}

static napi_value GetY(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, napix::unwrap_this_as<SceneNode>(env, info)->GetY());
}

static napi_value GetWidth(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, napix::unwrap_this_as<SceneNode>(env, info)->GetWidth());
}

static napi_value GetHeight(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, napix::unwrap_this_as<SceneNode>(env, info)->GetHeight());
}

static napi_value GetHidden(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, napix::unwrap_this_as<SceneNode>(env, info)->IsHidden());
}

static napi_value SetHidden(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<SceneNode>(env)};

  napi_value hidden{};
  if (ci[0] && napi_coerce_to_bool(env, ci[0], &hidden) == napi_ok) {
    node->SetHidden(hidden);
  }

  return {};
}

static napi_value AppendChild(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<SceneNode>(env)};
  auto child{ToSceneNode(env, ci[0])};

  if (child) {
    try {
      node->AppendChild(child);
    } catch (const std::exception& e) {
      napix::throw_error(env, e.what());
    }
  } else {
    napix::throw_error(env, "expected CSceneNode instance");
  }

  return {};
}

static napi_value InsertBefore(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<2>(env, info)};
  auto node{ci.unwrap_this_as<SceneNode>(env)};
  auto child{ToSceneNode(env, ci[0])};
  auto before{ToSceneNode(env, ci[1])};

  if (child && before) {
    try {
      node->InsertBefore(child, before);
    } catch (const std::exception& e) {
      napix::throw_error(env, e.what());
    }
  } else {
    napix::throw_error(env, "expected CSceneNode instance");
  }

  return {};
}

static napi_value RemoveChild(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<SceneNode>(env)};
  auto child{ToSceneNode(env, ci[0])};

  if (child) {
    node->RemoveChild(napix::unwrap_as<SceneNode>(env, ci[0]));
  }

  return {};
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  auto node{napix::unwrap_this_as<SceneNode>(env, info)};

  try {
    node->Destroy();
  } catch (const std::exception& e) {
    napix::throw_error(env, e.what());
  }

  return {};
}

static SceneNode* ToSceneNode(napi_env env, napi_value value) noexcept {
  // native inheritance does not work (https://github.com/nodejs/node-addon-api/issues/229)
  // workaround to do an instanceof to check for scene node
  if (Habitat::InstanceOf(env, value, Habitat::Class::CBoxSceneNode)
      || Habitat::InstanceOf(env, value, Habitat::Class::CImageSceneNode)
      || Habitat::InstanceOf(env, value, Habitat::Class::CTextSceneNode)) {
    return napix::unwrap_as<SceneNode>(env, value);
  }

  return {};
}

std::vector<napi_property_descriptor> CSceneNode::GetClassProperties(napi_env env) noexcept {
  return {
      instance_method("bindStyle", &BindStyle),
      instance_method("getX", &GetX),
      instance_method("getY", &GetY),
      instance_method("getWidth", &GetWidth),
      instance_method("getHeight", &GetHeight),
      instance_method("getHidden", &GetHidden),
      instance_method("setHidden", &SetHidden),
      instance_method("appendChild", &AppendChild),
      instance_method("insertBefore", &InsertBefore),
      instance_method("removeChild", &RemoveChild),
      instance_method("destroy", &Destroy),
  };
}

} // namespace bindings
} // namespace lse
