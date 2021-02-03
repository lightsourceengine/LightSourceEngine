/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "JSScene.h"

#include <napix.h>
#include <ObjectBuilder.h>

#include <lse/GraphicsContext.h>
#include <lse/RootSceneNode.h>
#include <lse/Scene.h>
#include <lse/bindings/JSStage.h>

namespace lse {
namespace bindings {

static napi_value Attach(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(napix::get_data<Scene>(env, info)->Attach())
  return {};
}

static napi_value Detach(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(napix::get_data<Scene>(env, info)->Detach())
  return {};
}

static napi_value Render(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(napix::get_data<Scene>(env, info)->Frame())
  return {};
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(napix::get_data<Scene>(env, info)->Destroy())
  return {};
}

static napi_value SetRoot(napi_env env, napi_callback_info info) noexcept {
  napi_value arg;

  napix::get_args(env, info, &arg);

  try {
    auto root = RootSceneNode::Cast(Napi::Value(env, arg));

    if (!root) {
      Napi::Error::New(env, "root arg must be a RootSceneNode instance").ThrowAsJavaScriptException();
      return {};
    }

    napix::get_data<Scene>(env, info)->SetRoot(root);
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
  }

  return {};
}

Napi::Value JSScene::New(const Napi::Env& env,
                         const Napi::Value& stage,
                         const Napi::Value& context) {
  auto stagePtr = napix::get_external_shared<Stage>(env, stage);
  NAPI_EXPECT_NOT_NULL(env, stagePtr, "stage arg must be an External");

  auto contextPtr = napix::get_external_shared<GraphicsContext>(env, context);
  NAPI_EXPECT_NOT_NULL(env, contextPtr, "context arg must be an External");

  auto scene = std::make_shared<Scene>(stagePtr, contextPtr);

  return Napi::ObjectBuilder(env, scene.get())
      .WithValue("$native", napix::new_external_shared(env, scene))
      .WithMethod("attach", &Attach)
      .WithMethod("detach", &Detach)
      .WithMethod("render", &Render)
      .WithMethod("destroy", &Destroy)
      .WithMethod("setRoot", &SetRoot)
      .Freeze()
      .ToObject();
}

} // namespace bindings
} // namespace lse
