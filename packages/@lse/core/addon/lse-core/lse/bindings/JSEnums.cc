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
  return ObjectBuilder(env)
      .WithValue("Undefined", StyleNumberUnitUndefined)
      .WithValue("Point", StyleNumberUnitPoint)
      .WithValue("Percent", StyleNumberUnitPercent)
      .WithValue("ViewportWidth", StyleNumberUnitViewportWidth)
      .WithValue("ViewportHeight", StyleNumberUnitViewportHeight)
      .WithValue("ViewportMin", StyleNumberUnitViewportMin)
      .WithValue("ViewportMax", StyleNumberUnitViewportMax)
      .WithValue("Auto", StyleNumberUnitAuto)
      .WithValue("Anchor", StyleNumberUnitAnchor)
      .WithValue("RootEm", StyleNumberUnitRootEm)
      .WithValue("Radian", StyleNumberUnitRadian)
      .WithValue("Degree", StyleNumberUnitDegree)
      .WithValue("Gradian", StyleNumberUnitGradian)
      .WithValue("Turn", StyleNumberUnitTurn)
      .Freeze()
      .ToObject();
}

Napi::Object NewStyleTransformEnum(Napi::Env env) {
  return ObjectBuilder(env)
      .WithValue("Identity", StyleTransformIdentity)
      .WithValue("Translate", StyleTransformTranslate)
      .WithValue("Rotate", StyleTransformRotate)
      .WithValue("Scale", StyleTransformScale)
      .Freeze()
      .ToObject();
}

Napi::Object NewStyleAnchorEnum(Napi::Env env) {
  return ObjectBuilder(env)
      .WithValue("Left", StyleAnchorLeft)
      .WithValue("Right", StyleAnchorRight)
      .WithValue("Center", StyleAnchorCenter)
      .WithValue("Top", StyleAnchorTop)
      .WithValue("Bottom", StyleAnchorBottom)
      .Freeze()
      .ToObject();
}

Napi::Object NewPluginIdEnum(Napi::Env env) {
  return ObjectBuilder(env)
      .WithValue("SDL", kPluginPlatformSdl)
      .WithValue("REF", kPluginPlatformRef)
      .WithValue("SDL_AUDIO", kPluginAudioSdlAudio)
      .WithValue("SDL_MIXER", kPluginAudioSdlMixer)
      .WithValue("REF_AUDIO", kPluginRefAudio)
      .WithValue("NULL", kPluginNull)
      .Freeze()
      .ToObject();
}

} // namespace bindings
} // namespace lse
