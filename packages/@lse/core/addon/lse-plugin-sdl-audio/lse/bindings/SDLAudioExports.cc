/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLAudioExports.h"

#include <stdexcept>
#include <napix.h>
#include <lse/Habitat.h>
#include <lse/SDL2.h>
#include <lse/bindings/AudioExports.h>
#include <lse/SDLAudioPlugin.h>
#include <lse/SDLAudioSource.h>
#include <lse/SDLAudioDestination.h>

namespace lse {
namespace bindings {

static napi_value PluginConstructor(napi_env env, napi_callback_info info) noexcept {
  return napix::js_class::constructor_helper(
      env,
      info,
      [](napi_env env, napi_callback_info info) -> void* {
        auto ci{napix::get_callback_info<1>(env, info)};

        return new (std::nothrow) SDLAudioPlugin(ToAudioPluginConfig(env, ci[0]));
      },
      [](napi_env env, void* instance, void* hint) {
        delete static_cast<SDLAudioPlugin*>(instance);
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

napi_value CreateSDLAudioPlugin(napi_env env, napi_value options) noexcept {
  if (Habitat::HasClass(env, Habitat::Class::CSampleAudioSource)
      || Habitat::HasClass(env, Habitat::Class::CSampleAudioDestination)) {
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

  // Create Plugin instance from class
  auto pluginConstructor{CreateAudioPluginClass(env, &PluginConstructor, "SDLAudioPlugin")};
  napi_value plugin{};
  size_t argc = options ? 1 : 0;
  napi_value argv[1]{options};

  napi_new_instance(env, pluginConstructor, argc, argv, &plugin);

  if (!plugin || napix::has_pending_exception(env)) {
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
    SDL2::Close();
    return {};
  }

  // Install helper classes into the env
  Habitat::SetClass(env, Habitat::Class::CSampleAudioSource,
      CreateAudioSourceClass(env, &Constructor<SDLAudioSource>, "SDLSampleAudioSource"));
  Habitat::SetClass(env, Habitat::Class::CSampleAudioDestination,
      CreateAudioDestinationClass(env, &Constructor<SDLAudioDestination>, "SDLSampleAudioDestination"));

  return plugin;
}

} // namespace bindings
} // namespace lse
