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

#pragma once

#include <vector>
#include <lse/Habitat.h>

namespace lse {

struct ImageRequest;

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

class CImage {
 public:
  static constexpr auto NAME = "CImage";
  static constexpr auto CLASS_ID = Habitat::Class::CImage;

  bool UnboxImageRequest(napi_env env, napi_value value, ImageRequest& imageRequest) noexcept;
};

class CImageManager {
 public:
  static constexpr auto NAME = "CImageManager";
  static constexpr auto CLASS_ID = Habitat::Class::CImageManager;

  static napi_value CreateClass(napi_env env) noexcept;
};

} // namespace bindings
} // namespace lse
