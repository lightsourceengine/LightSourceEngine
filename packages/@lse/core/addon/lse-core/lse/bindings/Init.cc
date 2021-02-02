/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <event/event.h>
#include <lse/Style.h>
#include <lse/StyleValidator.h>
#include <lse/BoxSceneNode.h>
#include <lse/ImageSceneNode.h>
#include <lse/LinkSceneNode.h>
#include <lse/RootSceneNode.h>
#include <lse/TextSceneNode.h>
#include <lse/System.h>
#include <lse/Log.h>
#include <lse/RefGraphicsContextImpl.h>
#include <lse/bindings/Bindings.h>
#include <lse/bindings/JSEnums.h>
#include <lse/bindings/JSScene.h>
#include <lse/bindings/JSStage.h>
#include <lse/bindings/JSStyle.h>
#include <lse/bindings/JSStyleClass.h>
#include <lse/bindings/JSStyleTransformSpec.h>
#include <lse/bindings/JSStyleValue.h>
#include <lse/bindings/Logger.h>
#include <napi.h>

#if defined(LSE_ENABLE_NATIVE_TESTS)
#include <test/LightSourceTestSuite.h>
#endif

using Napi::Env;
using Napi::Function;
using Napi::HandleScope;
using Napi::Object;
using facebook::yoga::Event;

void ExportFunction(Object exports, const Function& function) {
  exports.Set(function.Get("name").ToString(), function);
}

void ExportClass(Object exports, const Function& constructor) {
  ExportFunction(exports, constructor);
}

Object Init(Env env, Object exports) {
  HandleScope scope(env);

  auto logLevel = lse::GetEnvOrDefault("LSE_LOG_LEVEL", "INFO");

  if (!lse::SetLogLevel(logLevel)) {
    lse::SetLogLevel(lse::LogLevelInfo);
    LOG_ERROR("LSE_LOG_LEVEL contains invalid value of %s. Defaulting to INFO.", logLevel);
  }

  lse::Style::Init();
  lse::StyleValidator::Init();
  lse::StylePropertyValueInit();

  Event::subscribe(lse::SceneNode::YogaNodeLayoutEvent);

  exports["LogLevel"] = lse::bindings::NewLogLevelEnum(env);
  exports["StyleTransform"] = lse::bindings::NewStyleTransformEnum(env);
  exports["StyleUnit"] = lse::bindings::NewStyleUnitEnum(env);
  exports["StyleAnchor"] = lse::bindings::NewStyleAnchorEnum(env);
  exports["PluginId"] = lse::bindings::NewPluginIdEnum(env);

  ExportClass(exports, lse::bindings::JSStage::GetClass(env));
  ExportClass(exports, lse::bindings::JSScene::GetClass(env));
  ExportClass(exports, lse::bindings::JSStyle::GetClass(env));
  ExportClass(exports, lse::bindings::JSStyleClass::GetClass(env));
  ExportClass(exports, lse::bindings::JSStyleValue::GetClass(env));
  ExportClass(exports, lse::bindings::JSStyleTransformSpec::GetClass(env));

  ExportClass(exports, lse::BoxSceneNode::GetClass(env));
  ExportClass(exports, lse::ImageSceneNode::GetClass(env));
  ExportClass(exports, lse::LinkSceneNode::GetClass(env));
  ExportClass(exports, lse::TextSceneNode::GetClass(env));
  ExportClass(exports, lse::RootSceneNode::GetClass(env));

  ExportFunction(exports, Function::New(env, &lse::bindings::ParseColor, "parseColor"));
  ExportFunction(exports, Function::New(env, &lse::bindings::LoadSDLPlugin, "loadSDLPlugin"));
  ExportFunction(exports, Function::New(env, &lse::bindings::LoadSDLAudioPlugin, "loadSDLAudioPlugin"));
  ExportFunction(exports, Function::New(env, &lse::bindings::LoadSDLMixerPlugin, "loadSDLMixerPlugin"));
  ExportFunction(exports, Function::New(env, &lse::SceneNode::GetInstanceCount, "getSceneNodeInstanceCount"));

  // TODO: temporary patch to get ref plugin working with new plugin strategy
  auto createRefGraphicsContext = [](const Napi::CallbackInfo& info) -> Napi::Value {
    return lse::GraphicsContext::Create<lse::RefGraphicsContextImpl>(info.Env(), info[0]);
  };
  ExportFunction(exports, Function::New(env, createRefGraphicsContext, "createRefGraphicsContext"));

  exports["logger"] = lse::bindings::NewLoggerObject(env);
  exports["styleProperties"] = lse::bindings::GetStyleProperties(env);

#if defined(LSE_ENABLE_NATIVE_TESTS)
  exports["test"] = lse::LightSourceTestSuite(env);
#endif

  return exports;
}

NODE_API_MODULE(LightSourceEngineCore, Init);
