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
  auto ci{ napix::get_callback_info<4>(env, info) };
  Stage* stage{};

  if (Habitat::InstanceOf(env, ci[0], Habitat::Class::CStage)) {
    stage = unwrap_as<Stage>(env, ci[0]);
  }

  NAPIX_EXPECT_NOT_NULL(env, stage, "expected CStage instance", {});

  FontManager* fontManager{};

  if (Habitat::InstanceOf(env, ci[1], Habitat::Class::CFontManager)) {
    fontManager = unwrap_as<FontManager>(env, ci[1]);
  }

  NAPIX_EXPECT_NOT_NULL(env, fontManager, "expected CFontManager instance", {});

  ImageManager* imageManager{};

  if (Habitat::InstanceOf(env, ci[2], Habitat::Class::CImageManager)) {
    imageManager = unwrap_as<ImageManager>(env, ci[2]);
  }

  NAPIX_EXPECT_NOT_NULL(env, imageManager, "expected CImageManager instance", {});

  GraphicsContext* context{};

  if (Habitat::InstanceOf(env, ci[3], Habitat::Class::CGraphicsContext)) {
    context = unwrap_as<GraphicsContext>(env, ci[3]);
  }

  NAPIX_EXPECT_NOT_NULL(env, context, "expected CGraphicsContext instance", {});

  return new Scene(stage, fontManager, imageManager, context);
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

  RootSceneNode* root{};

  if (Habitat::InstanceOf(env, ci[0], Habitat::Class::CRootSceneNode)) {
    root = napix::unwrap_as<RootSceneNode>(env, ci[0]);
  }

  NAPIX_EXPECT_NOT_NULL(env, root, "root arg is not a CRootSceneNode instance", {});

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
