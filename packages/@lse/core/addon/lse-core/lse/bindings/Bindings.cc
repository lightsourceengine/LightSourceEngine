/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/bindings/Bindings.h>

#include <lse/StyleEnums.h>
#include <lse/bindings/Convert.h>
#include <lse/PlatformPlugin.h>
#include <lse/SDLPlatformPluginImpl.h>
#include <lse/RefPlatformPluginImpl.h>
#include <lse/AudioPlugin.h>
#include <lse/RefAudioPluginImpl.h>
#include <lse/SDLAudioPluginImpl.h>
#include <lse/SDLMixerAudioPluginImpl.h>
#include <lse/System.h>
#include <lse/SDL2.h>
#include <lse/SDL2_mixer.h>
#include <lse/Config.h>
#include <std17/filesystem>

namespace lse {
namespace bindings {

static void EnsureSDL2(const Napi::Env& env);
static void EnsureSDL2_mixer(const Napi::Env& env);

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

Napi::Value LoadPlugin(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    auto plugin = Napi::Object::New(info.Env());
    auto name = Napi::CopyUtf8(info[0]);

    if (kEnablePluginPlatformSdl && strcmp(name, kPluginPlatformSdl) == 0) {
        EnsureSDL2(env);
        return PlatformPluginInit<lse::SDLPlatformPluginImpl>(env, plugin, kPluginPlatformSdl);
    } else if (kEnablePluginPlatformRef && strcmp(name, kPluginPlatformRef) == 0) {
        return PlatformPluginInit<lse::RefPlatformPluginImpl>(env, plugin, kPluginPlatformRef);
    } else if (kEnablePluginAudioSdlAudio && strcmp(name, kPluginAudioSdlAudio) == 0) {
        EnsureSDL2(env);
        return lse::AudioPluginInit<lse::SDLAudioPluginImpl>(env, plugin, kPluginAudioSdlAudio);
    } else if (kEnablePluginAudioSdlMixer && strcmp(name, kPluginAudioSdlMixer) == 0) {
        EnsureSDL2(env);
        EnsureSDL2_mixer(env);
        return lse::AudioPluginInit<lse::SDLMixerAudioPluginImpl>(env, plugin, kPluginAudioSdlMixer);
    } else if (kEnablePluginAudioRef && strcmp(name, kPluginAudioRef) == 0) {
        return lse::AudioPluginInit<lse::RefAudioPluginImpl>(env, plugin, kPluginAudioRef);
    } else {
        throw Napi::Error::New(env, "Unknown plugin name");
    }
}

static void EnsureSDL2(const Napi::Env& env) {
    if (SDL2::IsOpen()) {
        return;
    }

    if (kIsMac && !HasEnv("LS_SDL_USE_DYLIB")) {
        std17::filesystem::path p(GetEnvOrDefault("LS_RUNTIME_FRAMEWORK_PATH", kDefaultRuntimeFrameworkPath));

        NAPI_TRY(env, SDL2::Open(p.append(kSDLFramework).append(kSDLFrameworkLib).c_str()));
    } else {
        NAPI_TRY(env, SDL2::Open(GetEnvOrDefault("LS_SDL_LIB_NAME", kSDLDefaultLibName)));
    }
}

static void EnsureSDL2_mixer(const Napi::Env& env) {
    if (SDL2::mixer::IsOpen()) {
        return;
    }

    if (kIsMac && !HasEnv("LS_SDL_MIXER_USE_DYLIB")) {
        std17::filesystem::path p(GetEnvOrDefault("LS_RUNTIME_FRAMEWORK_PATH", kDefaultRuntimeFrameworkPath));

        NAPI_TRY(env, SDL2::mixer::Open(p.append(kSDLMixerFramework).append(kSDLMixerFrameworkLib).c_str()));
    } else {
        NAPI_TRY(env, SDL2::mixer::Open(GetEnvOrDefault("LS_SDL_MIXER_LIB_NAME", kSDLMixerDefaultLibName)));
    }
}

} // namespace bindings
} // namespace lse
