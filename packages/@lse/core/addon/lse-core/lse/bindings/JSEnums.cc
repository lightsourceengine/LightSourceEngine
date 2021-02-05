/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/StyleEnums.h>
#include <lse/bindings/JSEnums.h>
#include <lse/Config.h>
#include <ObjectBuilder.h>

using Napi::ObjectBuilder;

namespace lse {
namespace bindings {

Napi::Object NewStyleUnitEnum(Napi::Env env) {
  auto attributes{ napi_enumerable };

  return ObjectBuilder(env)
      .WithValue("Undefined", StyleNumberUnitUndefined, attributes)
      .WithValue("Point", StyleNumberUnitPoint, attributes)
      .WithValue("Percent", StyleNumberUnitPercent, attributes)
      .WithValue("ViewportWidth", StyleNumberUnitViewportWidth, attributes)
      .WithValue("ViewportHeight", StyleNumberUnitViewportHeight, attributes)
      .WithValue("ViewportMin", StyleNumberUnitViewportMin, attributes)
      .WithValue("ViewportMax", StyleNumberUnitViewportMax, attributes)
      .WithValue("Auto", StyleNumberUnitAuto, attributes)
      .WithValue("Anchor", StyleNumberUnitAnchor, attributes)
      .WithValue("RootEm", StyleNumberUnitRootEm, attributes)
      .WithValue("Radian", StyleNumberUnitRadian, attributes)
      .WithValue("Degree", StyleNumberUnitDegree, attributes)
      .WithValue("Gradian", StyleNumberUnitGradian, attributes)
      .WithValue("Turn", StyleNumberUnitTurn, attributes)
      .Freeze()
      .ToObject();
}

Napi::Object NewStyleTransformEnum(Napi::Env env) {
  auto attributes{ napi_enumerable };

  return ObjectBuilder(env)
      .WithValue("Identity", StyleTransformIdentity, attributes)
      .WithValue("Translate", StyleTransformTranslate, attributes)
      .WithValue("Rotate", StyleTransformRotate, attributes)
      .WithValue("Scale", StyleTransformScale, attributes)
      .Freeze()
      .ToObject();
}

Napi::Object NewStyleAnchorEnum(Napi::Env env) {
  auto attributes{ napi_enumerable };

  return ObjectBuilder(env)
      .WithValue("Left", StyleAnchorLeft, attributes)
      .WithValue("Right", StyleAnchorRight, attributes)
      .WithValue("Center", StyleAnchorCenter, attributes)
      .WithValue("Top", StyleAnchorTop, attributes)
      .WithValue("Bottom", StyleAnchorBottom, attributes)
      .Freeze()
      .ToObject();
}

Napi::Object NewPluginIdEnum(Napi::Env env) {
  auto attributes{ napi_enumerable };

  return ObjectBuilder(env)
      .WithValue("SDL", kPluginPlatformSdl, attributes)
      .WithValue("REF", kPluginPlatformRef, attributes)
      .WithValue("SDL_AUDIO", kPluginAudioSdlAudio, attributes)
      .WithValue("SDL_MIXER", kPluginAudioSdlMixer, attributes)
      .WithValue("REF_AUDIO", kPluginRefAudio, attributes)
      .WithValue("NULL", kPluginNull, attributes)
      .Freeze()
      .ToObject();
}

} // namespace bindings
} // namespace lse
