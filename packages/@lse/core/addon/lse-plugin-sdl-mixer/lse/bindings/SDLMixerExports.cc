/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLMixerExports.h"

#include <stdexcept>
#include <napix.h>
#include <lse/Habitat.h>
#include <lse/SDL2.h>
#include <lse/SDL2_mixer.h>
#include <lse/SDLMixerAudioPlugin.h>
#include <lse/SDLMixerAudioSource.h>
#include <lse/SDLMixerAudioDestination.h>
#include <lse/bindings/AudioExports.h>

namespace lse {
namespace bindings {

static napi_value PluginConstructor(napi_env env, napi_callback_info info) noexcept {
  return napix::js_class::constructor_helper(
      env,
      info,
      [](napi_env env, napi_callback_info info) -> void* {
        auto ci{napix::get_callback_info<1>(env, info)};

        return new (std::nothrow) SDLMixerAudioPlugin(ToAudioPluginConfig(env, ci[0]));
      },
      [](napi_env env, void* instance, void* hint) {
        delete static_cast<SDLMixerAudioPlugin*>(instance);
      });
}

template<class T>
static napi_value Constructor(napi_env env, napi_callback_info info) noexcept {
  return napix::js_class::constructor_helper(
      env,
      info,
      [](napi_env env, napi_callback_info info) -> void* {
        return new (std::nothrow) T();
      },
      [](napi_env env, void* instance, void* hint) {
        delete static_cast<T*>(instance);
      });
}

napi_value CreateSDLMixerAudioPlugin(napi_env env, napi_value options) noexcept {
  if (Habitat::HasClass(env, Habitat::Class::CSampleAudioSource)
      || Habitat::HasClass(env, Habitat::Class::CSampleAudioDestination)
      || Habitat::HasClass(env, Habitat::Class::CStreamAudioSource)
      || Habitat::HasClass(env, Habitat::Class::CStreamAudioDestination)) {
    napix::throw_error(env, "audio plugin already installed?");
    return {};
  }

  // Init SDL
  try {
    SDL2::Open();
  } catch (const std::exception& e) {
    napix::throw_error(env, e.what());
    return {};
  }

  // Init SDL Mixer
  try {
    SDL2::mixer::Open();
  } catch (const std::exception& e) {
    SDL2::Close();
    napix::throw_error(env, e.what());
    return {};
  }

  // Create Plugin instance from class
  auto pluginConstructor{CreateAudioPluginClass(env, &PluginConstructor, "SDLMixerAudioPlugin")};
  napi_value plugin{};
  size_t argc = options ? 1 : 0;
  napi_value argv[]{options};
  napi_new_instance(env, pluginConstructor, argc, argv, &plugin);

  if (!plugin || napix::has_pending_exception(env)) {
    SDL2::mixer::Close();
    SDL2::Close();
    return{};
  }

  // Try to attach the plugin, as init may fail and the app can fall back to another audio plugin
  auto nativePlugin{napix::unwrap_as<AudioPlugin>(env, plugin)};

  try {
    nativePlugin->Attach();
  } catch (const std::exception& e) {
    nativePlugin->Destroy();
    napix::throw_error(env, e.what());
    SDL2::mixer::Close();
    SDL2::Close();
    return {};
  }

  // Install helper classes into the env
  Habitat::SetClass(env, Habitat::Class::CSampleAudioSource,
      CreateAudioSourceClass(env, &Constructor<SDLMixerSampleSource>, "SDLMixerSampleSource"));
  Habitat::SetClass(env, Habitat::Class::CSampleAudioDestination,
      CreateAudioDestinationClass(env, &Constructor<SDLMixerSample>, "SDLMixerSampleAudioDestination"));
  Habitat::SetClass(env, Habitat::Class::CStreamAudioSource,
      CreateAudioSourceClass(env, &Constructor<SDLMixerStreamSource>, "SDLMixerStreamSource"));
  Habitat::SetClass(env, Habitat::Class::CStreamAudioDestination,
      CreateAudioDestinationClass(env, &Constructor<SDLMixerStream>, "SDLMixerStreamAudioDestination"));

  return plugin;
}

} // namespace bindings
} // namespace lse
