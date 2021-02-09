/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CoreExports.h"

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

using NapiFunction = Napi::Value (*)(const Napi::CallbackInfo&);

static napi_value Function(napi_env env, const char* name, NapiFunction func) noexcept;
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
  Export(env, exports, Function(env, "loadSDLAudioPlugin", &LoadSDLAudioPlugin));
  Export(env, exports, Function(env, "loadSDLMixerPlugin", &LoadSDLMixerPlugin));

  Export(env, exports, "lockStyle", &LockStyle);
  Export(env, exports, "resetStyle", &ResetStyle);
  Export(env, exports, "setStyleParent", &SetStyleParent);
  Export(env, exports, "installStyleValue", &InstallStyleValue);
  Export(env, exports, "installStyleTransformSpec", &InstallStyleTransformSpec);

  Export(env, exports, Function(env, "parseColor", &ParseColor));
  Export(env, exports, Function(env, "getSceneNodeInstanceCount", &SceneNode::GetInstanceCount));

  // Classes
  LoadHabitatClasses(env);
  Export(env, exports, CStage::CLASS_ID);
  Export(env, exports, CScene::CLASS_ID);
  Export(env, exports, CFontManager::CLASS_ID);
  Export(env, exports, CStyle::CLASS_ID);
  Export(env, exports, JSStyleValue::GetClass(env));
  Export(env, exports, JSStyleTransformSpec::GetClass(env));
  Export(env, exports, BoxSceneNode::GetClass(env));
  Export(env, exports, ImageSceneNode::GetClass(env));
  Export(env, exports, TextSceneNode::GetClass(env));
  Export(env, exports, RootSceneNode::GetClass(env));

  return exports;
}

static napi_value Function(napi_env env, const char* name, NapiFunction func) noexcept {
  try {
    return Napi::Function::New(env, func, name);
  } catch (const Napi::Error& e) {
    // TODO: rethrow napi error
    return {};
  }
}

static void Export(napi_env env, napi_value exports, const char* name, napi_value value) noexcept {
  napi_set_named_property(env, exports, name, value);
}

static void Export(napi_env env, napi_value exports, const char* name, napi_callback func) noexcept {
  napi_value value{};

  napi_create_function(env, name, NAPI_AUTO_LENGTH, func, nullptr, &value);

  Export(env, exports, name, value);
}

static void Export(napi_env env, napi_value exports, napi_value value) noexcept {
  napi_value name{};

  if (napi_get_named_property(env, value, "name", &name) != napi_ok || !name) {
    return;
  }

  napi_set_property(env, exports, name, value);
}

static void Export(napi_env env, napi_value exports, Habitat::Class::Enum classId) noexcept {
  return Export(env, exports, Habitat::GetClass(env, classId));
}

static void LoadHabitatClasses(napi_env env) noexcept {
  Habitat::SetClass(env, Habitat::Class::CStage, CStage::CreateClass(env));
  Habitat::SetClass(env, Habitat::Class::CScene, CScene::CreateClass(env));
  Habitat::SetClass(env, Habitat::Class::CFontManager, CFontManager::CreateClass(env));
  Habitat::SetClass(env, Habitat::Class::CStyle, CStyle::CreateClass(env));
}

} // namespace bindings
} // namespace lse
