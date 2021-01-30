/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CapabilitiesView.h"

using Napi::Array;
using Napi::Number;
using Napi::Object;
using Napi::String;

namespace lse {

Napi::Object ToCapabilitiesView(Napi::Env env, const Capabilities& caps) {
  auto jsCaps{ Object::New(env) };
  auto jsDisplays{ Array::New(env, caps.displays.size()) };
  auto jsVideoDrivers{ Array::New(env, caps.videoDrivers.size()) };
  auto i{ 0u };
  auto toObject{
      [](Napi::Env env, const DisplayMode& mode) -> Object {
        auto modeObj{ Object::New(env) };

        modeObj["width"] = mode.width;
        modeObj["height"] = mode.height;

        return modeObj;
      }
  };

  for (const auto& display : caps.displays) {
    auto jsDisplay{ Object::New(env) };
    auto jsModes{ Array::New(env, display.modes.size()) };
    auto j{ 0 };

    jsDisplay["id"] = Number::New(env, display.id);
    jsDisplay["name"] = String::New(env, display.name);
    jsDisplay["defaultMode"] = toObject(env, display.defaultMode);

    for (const DisplayMode& displayMode : display.modes) {
      jsModes[j++] = toObject(env, displayMode);
    }

    jsDisplay["modes"] = jsModes;

    jsDisplays[i++] = jsDisplay;
  }

  jsCaps["displays"] = jsDisplays;
  i = 0;

  for (const auto& videoDriver : caps.videoDrivers) {
    jsVideoDrivers[i++] = Napi::String::New(env, videoDriver);
  }

  jsCaps["videoDrivers"] = jsVideoDrivers;

  return jsCaps;
}

} // namespace lse
