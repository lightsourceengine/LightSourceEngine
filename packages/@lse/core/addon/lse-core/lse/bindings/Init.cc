/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <event/event.h>
#include <lse/Style.h>
#include <lse/StyleValidator.h>
#include <lse/System.h>
#include <lse/SceneNode.h>
#include <lse/Log.h>
#include <lse/FTFontDriver.h>
#include <lse/Habitat.h>
#include <lse/bindings/CoreExports.h>
#include <node_api.h>

using facebook::yoga::Event;

static napi_value Init(napi_env env, napi_value exports) {
  auto logLevel = lse::GetEnvOrDefault("LSE_LOG_LEVEL", "INFO");

  if (!lse::SetLogLevel(logLevel)) {
    lse::SetLogLevel(lse::LogLevelInfo);
    LOG_ERROR("LSE_LOG_LEVEL contains invalid value of %s. Defaulting to INFO.", logLevel);
  }

  if (!lse::Habitat::Init(env)) {
    // TODO: throw?
    return {};
  }

  lse::Habitat::SetAppData(
      env,
      lse::FontDriver::APP_DATA_KEY,
      new lse::FTFontDriver(),
      [](void* data) {
        delete static_cast<lse::FTFontDriver*>(data);
      });

  lse::Style::Init();
  lse::StyleValidator::Init();
  lse::StylePropertyValueInit();
  Event::subscribe(lse::SceneNode::YogaNodeLayoutEvent);

  lse::bindings::CoreExports(env, exports);

  return exports;
}

extern "C" {
static napi_module _module = {
    1,
    0,
    "lse-core.node",
    &Init,
    "LightSourceEngineCore",
    nullptr,
    { nullptr }
};

NAPI_C_CTOR (_register_Init) {
  napi_module_register(&_module);
}
}
