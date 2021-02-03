/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/bindings/Bindings.h>

#include <lse/StyleEnums.h>
#include <lse/bindings/Convert.h>
#include <lse/AudioPlugin.h>
#include <lse/SDLAudioPluginImpl.h>
#include <lse/SDLMixerAudioPluginImpl.h>
#include <lse/SDL2.h>
#include <lse/SDL2_mixer.h>
#include <lse/Config.h>
#include <lse/bindings/JSGraphicsContext.h>
#include <lse/RefGraphicsContext.h>
#include <lse/bindings/SDLPlatformPluginExports.h>

namespace lse {
namespace bindings {

Napi::Value ParseColor(const Napi::CallbackInfo& info) {
  auto env{ info.Env() };

  return BoxColor(env, UnboxColor(env, info[0]));
}

Napi::Object GetStyleProperties(Napi::Env env) {
  auto styleProperties{ Napi::Object::New(env) };

  for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
    styleProperties[ToString(static_cast<StyleProperty>(i))] = Napi::Number::New(env, i);
  }

  return styleProperties;
}

Napi::Value LoadSDLPlugin(const Napi::CallbackInfo& info) {
  if (kEnablePluginPlatformSdl) {
    return SDLPlatformPluginExports(info.Env());
  } else {
    throw Napi::Error::New(info.Env(), "SDL plugin is not available.");
  }
}

Napi::Value LoadSDLAudioPlugin(const Napi::CallbackInfo& info) {
  if (kEnablePluginAudioSdlAudio) {
    SDL2::Open();
    return lse::AudioPluginInit<lse::SDLAudioPluginImpl>(info.Env());
  } else {
    throw Napi::Error::New(info.Env(), "SDL Audio plugin is not available.");
  }
}

Napi::Value LoadSDLMixerPlugin(const Napi::CallbackInfo& info) {
  if (kEnablePluginAudioSdlMixer) {
    SDL2::Open();
    SDL2::mixer::Open();
    return lse::AudioPluginInit<lse::SDLMixerAudioPluginImpl>(info.Env());
  } else {
    throw Napi::Error::New(info.Env(), "SDL Audio plugin is not available.");
  }
}

Napi::Value CreateRefGraphicsContext(const Napi::CallbackInfo& info) {
  return JSGraphicsContext::New<RefGraphicsContext>(info.Env(), info[0]);
}

} // namespace bindings
} // namespace lse
