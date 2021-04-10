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

NAPI_C_CTOR(_register_Init) {
  napi_module_register(&_module);
}
}
