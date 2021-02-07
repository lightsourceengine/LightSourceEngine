/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CoreClasses.h"

#include <napix.h>
#include <lse/GraphicsContext.h>
#include <lse/RootSceneNode.h>
#include <lse/Scene.h>
#include <lse/Habitat.h>
#include <lse/Stage.h>

using napix::unwrap_this_as;
using napix::unwrap_as;
using napix::js_class::define;
using napix::descriptor::instance_method;
using napix::js_class::constructor_helper;

namespace lse {
namespace bindings {

static void* CreateScene(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<2>(env, info) };
  Stage* stage{};

  if (Habitat::InstanceOf(env, ci[0], Habitat::Class::CStage)) {
    stage = unwrap_as<Stage>(env, ci[0]);
  }

  NAPIX_EXPECT_NOT_NULL(env, stage, "stage arg must be a CStage instance", {});

  GraphicsContext* context{};

  if (Habitat::InstanceOf(env, ci[1], Habitat::Class::CGraphicsContext)) {
    context = unwrap_as<GraphicsContext>(env, ci[1]);
  }

  NAPIX_EXPECT_NOT_NULL(env, context, "context arg must be a GraphicsContext instance", {});

  return new Scene(stage, context);
}

static napi_value Constructor(napi_env env, napi_callback_info info) noexcept {
  return constructor_helper(
      env,
      info,
      &CreateScene,
      [](napi_env env, void* data, void* hint) {
        static_cast<Scene*>(data)->Unref();
      });
}

static napi_value Attach(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(env, unwrap_this_as<Scene>(env, info)->Attach(), {});
  return {};
}

static napi_value Detach(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(env, unwrap_this_as<Scene>(env, info)->Detach(), {});
  return {};
}

static napi_value Render(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(env, unwrap_this_as<Scene>(env, info)->Frame(), {});
  return {};
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(env, unwrap_this_as<Scene>(env, info)->Destroy(), {})
  return {};
}

static napi_value SetRoot(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };
  auto root{ RootSceneNode::Cast(Napi::Value(env, ci[0])) };

  NAPIX_EXPECT_NOT_NULL(env, root, "root arg is not a RootSceneNode instance", {});

  unwrap_this_as<Scene>(env, info)->SetRoot(root);

  return {};
}

napi_value CScene::CreateClass(napi_env env) {
  return define(env, NAME, Constructor, {
      instance_method("attach", &Attach),
      instance_method("detach", &Detach),
      instance_method("render", &Render),
      instance_method("destroy", &Destroy),
      instance_method("setRoot", &SetRoot),
  });
}

} // namespace bindings
} // namespace lse
