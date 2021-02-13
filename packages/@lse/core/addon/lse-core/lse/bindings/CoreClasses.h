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
#include <lse/RootSceneNode.h>
#include <lse/TextSceneNode.h>

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

class CStyle {
 public:
  static constexpr auto NAME = "CStyle";
  static constexpr auto CLASS_ID = Habitat::Class::CStyle;

  static napi_value CreateClass(napi_env env);
};

class CRefGraphicsContext {
 public:
  static constexpr auto NAME = "CRefGraphicsContext";
  static constexpr auto CLASS_ID = Habitat::Class::CGraphicsContext;

  static napi_value CreateClass(napi_env env);
};

class CSceneNode {
 public:
  static constexpr auto NAME = "CSceneNode";

  // inheritance does not work with native class objects: https://github.com/nodejs/node-addon-api/issues/229
  // as a workaround, each scene node class has it's own copy of the CSceneNode base class properties
  static std::vector<napi_property_descriptor> GetClassProperties(napi_env env) noexcept;
};

class CBoxSceneNode {
 public:
  static constexpr auto NAME = "CBoxSceneNode";
  static constexpr auto CLASS_ID = Habitat::Class::CBoxSceneNode;

  static napi_value CreateClass(napi_env env) noexcept;
};

class CImageSceneNode {
 public:
  static constexpr auto NAME = "CImageSceneNode";
  static constexpr auto CLASS_ID = Habitat::Class::CImageSceneNode;

  static napi_value CreateClass(napi_env env) noexcept;
};

class CRootSceneNode {
 public:
  static constexpr auto NAME = "CRootSceneNode";
  static constexpr auto CLASS_ID = Habitat::Class::CRootSceneNode;

  static napi_value CreateClass(napi_env env) noexcept;
};

class CTextSceneNode {
 public:
  static constexpr auto NAME = "CTextSceneNode";
  static constexpr auto CLASS_ID = Habitat::Class::CTextSceneNode;

  static napi_value CreateClass(napi_env env) noexcept;
};

} // namespace bindings
} // namespace lse
