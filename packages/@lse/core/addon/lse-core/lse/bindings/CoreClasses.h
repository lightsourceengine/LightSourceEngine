/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <lse/Habitat.h>

#include <lse/BoxSceneNode.h>
#include <lse/ImageSceneNode.h>
#include <lse/LinkSceneNode.h>
#include <lse/RootSceneNode.h>
#include <lse/TextSceneNode.h>
#include <lse/bindings/JSStyle.h>
#include <lse/bindings/JSStyleClass.h>
#include <lse/bindings/JSStyleTransformSpec.h>
#include <lse/bindings/JSStyleValue.h>

namespace lse {
namespace bindings {

class CStage {
 public:
  static constexpr auto NAME = "CStage";
  static constexpr auto CLASS_ID = Habitat::Class::CStage;

  static napi_value CreateClass(napi_env env);
};

class CScene {
 public:
  static constexpr auto NAME = "CScene";
  static constexpr auto CLASS_ID = Habitat::Class::CScene;

  static napi_value CreateClass(napi_env env);
};

} // namespace bindings
} // namespace lse
