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
#include <lse/RefAudioPluginImpl.h>
#include <lse/SDLAudioPluginImpl.h>
#include <lse/SDLMixerAudioPluginImpl.h>
#include <lse/SDL2.h>
#include <lse/SDL2_mixer.h>
#include <lse/Config.h>
#include <lse/string-ext.h>
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

Napi::Value LoadPluginById(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  auto plugin = Napi::Object::New(info.Env());
  auto name = Napi::CopyUtf8(info[0]);

  if (kEnablePluginPlatformSdl && strcmp(name, kPluginPlatformSdl) == 0) {
    return SDLPlatformPluginExports(env);
  } else if (kEnablePluginAudioSdlAudio && strcmp(name, kPluginAudioSdlAudio) == 0) {
    SDL2::Open();
    return lse::AudioPluginInit<lse::SDLAudioPluginImpl>(env, plugin, kPluginAudioSdlAudio);
  } else if (kEnablePluginAudioSdlMixer && strcmp(name, kPluginAudioSdlMixer) == 0) {
    SDL2::Open();
    SDL2::mixer::Open();
    return lse::AudioPluginInit<lse::SDLMixerAudioPluginImpl>(env, plugin, kPluginAudioSdlMixer);
  } else if (kEnablePluginAudioRef && strcmp(name, kPluginRefAudio) == 0) {
    return lse::AudioPluginInit<lse::RefAudioPluginImpl>(env, plugin, kPluginRefAudio);
  } else {
    throw Napi::Error::New(env, Format("Unknown plugin name: %s", name));
  }
}

} // namespace bindings
} // namespace lse
