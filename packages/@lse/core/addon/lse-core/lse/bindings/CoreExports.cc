/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CoreExports.h"

#include <napix.h>
#include <lse/bindings/CoreFunctions.h>
#include <lse/bindings/CoreClasses.h>
#include <lse/bindings/CoreEnums.h>
#include <lse/bindings/LoggerExports.h>
#include <lse/bindings/FontExports.h>
#if defined(LSE_ENABLE_NATIVE_TESTS)
#include <test/LightSourceTestSuite.h>
#endif

namespace lse {
namespace bindings {

static void Export(napi_env env, napi_value exports, const char* name, napi_value value) noexcept;
static void Export(napi_env env, napi_value exports, const char* name, napi_callback func) noexcept;
static void Export(napi_env env, napi_value exports, napi_value value) noexcept;
static void Export(napi_env env, napi_value exports, Habitat::Class::Enum classId) noexcept;
static void LoadHabitatClasses(napi_env env) noexcept;

napi_value CoreExports(napi_env env, napi_value exports) noexcept {
  // Enums
  Export(env, exports, "LogLevel", NewLogLevelEnum(env));
  Export(env, exports, "StyleTransform", NewStyleTransformEnum(env));
  Export(env, exports, "StyleUnit", NewStyleUnitEnum(env));
  Export(env, exports, "StyleAnchor", NewStyleAnchorEnum(env));
  Export(env, exports, "FontStatus", NewFontStatusEnum(env));
  Export(env, exports, "FontStyle", NewFontStyleEnum(env));
  Export(env, exports, "FontWeight", NewFontWeightEnum(env));

  // Objects
  Export(env, exports, "logger", NewLoggerObject(env));
#if defined(LSE_ENABLE_NATIVE_TESTS)
  Export(env, exports, "test", LightSourceTestSuite(env));
#endif

  // Functions
  Export(env, exports, "loadSDLPlugin", &LoadSDLPlugin);
  Export(env, exports, "loadRefPlugin", &LoadRefPlugin);
  Export(env, exports, "loadSDLAudioPlugin", &LoadSDLAudioPlugin);
  Export(env, exports, "loadSDLMixerPlugin", &LoadSDLMixerPlugin);

  Export(env, exports, "lockStyle", &LockStyle);
  Export(env, exports, "resetStyle", &ResetStyle);
  Export(env, exports, "setStyleParent", &SetStyleParent);
  Export(env, exports, "installStyleValue", &InstallStyleValue);
  Export(env, exports, "installStyleTransformSpec", &InstallStyleTransformSpec);

  Export(env, exports, "parseValue", &ParseValue);
  Export(env, exports, "parseColor", &ParseColor);

  Export(env, exports, "getSceneNodeInstanceCount", &GetSceneNodeInstanceCount);

  // Classes
  LoadHabitatClasses(env);
  Export(env, exports, CStage::CLASS_ID);
  Export(env, exports, CScene::CLASS_ID);
  Export(env, exports, CFontManager::CLASS_ID);
  Export(env, exports, CStyle::CLASS_ID);
  Export(env, exports, CBoxSceneNode::CLASS_ID);
  Export(env, exports, CImageSceneNode::CLASS_ID);
  Export(env, exports, CRootSceneNode::CLASS_ID);
  Export(env, exports, CTextSceneNode::CLASS_ID);

  return exports;
}

static void Export(napi_env env, napi_value exports, const char* name, napi_value value) noexcept {
  if (napix::has_pending_exception(env)) {
    return;
  }

  napi_set_named_property(env, exports, name, value);
}

static void Export(napi_env env, napi_value exports, const char* name, napi_callback func) noexcept {
  if (napix::has_pending_exception(env)) {
    return;
  }

  napi_value value{};

  napi_create_function(env, name, NAPI_AUTO_LENGTH, func, nullptr, &value);

  Export(env, exports, name, value);
}

static void Export(napi_env env, napi_value exports, napi_value value) noexcept {
  if (napix::has_pending_exception(env)) {
    return;
  }

  napi_value name{};

  if (napi_get_named_property(env, value, "name", &name) != napi_ok || !name) {
    return;
  }

  napi_set_property(env, exports, name, value);
}

static void Export(napi_env env, napi_value exports, Habitat::Class::Enum classId) noexcept {
  if (napix::has_pending_exception(env)) {
    return;
  }

  return Export(env, exports, Habitat::GetClass(env, classId));
}

static void LoadHabitatClasses(napi_env env) noexcept {
  Habitat::SetClass(env, CStage::CLASS_ID, CStage::CreateClass(env));
  Habitat::SetClass(env, CScene::CLASS_ID, CScene::CreateClass(env));
  Habitat::SetClass(env, CFontManager::CLASS_ID, CFontManager::CreateClass(env));
  Habitat::SetClass(env, CStyle::CLASS_ID, CStyle::CreateClass(env));
  Habitat::SetClass(env, CBoxSceneNode::CLASS_ID, CBoxSceneNode::CreateClass(env));
  Habitat::SetClass(env, CImageSceneNode::CLASS_ID, CImageSceneNode::CreateClass(env));
  Habitat::SetClass(env, CRootSceneNode::CLASS_ID, CRootSceneNode::CreateClass(env));
  Habitat::SetClass(env, CTextSceneNode::CLASS_ID, CTextSceneNode::CreateClass(env));
}

} // namespace bindings
} // namespace lse
