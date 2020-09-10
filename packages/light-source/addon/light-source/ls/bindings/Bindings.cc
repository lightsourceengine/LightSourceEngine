/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/bindings/Bindings.h>

#include <ls/StyleEnums.h>
#include <ls/bindings/Convert.h>
#include <ls/PlatformPlugin.h>
#include <ls/SDLPlatformPluginImpl.h>
#include <ls/RefPlatformPluginImpl.h>
#include <ls/AudioPlugin.h>
#include <ls/RefAudioPluginImpl.h>
#include <ls/SDLAudioPluginImpl.h>
#include <ls/SDLMixerAudioPluginImpl.h>
#include <ls/System.h>
#include <ls/SDL2.h>
#include <ls/SDL2_mixer.h>
#include <std17/filesystem>
#include <ls/Config.h>

namespace ls {
namespace bindings {

static void EnsureSDL2(const Napi::Env& env);
static void EnsureSDL2_mixer(const Napi::Env& env);
static bool LoadFramework(const char* framework, const char* frameworkLib);

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
        return PlatformPluginInit<ls::SDLPlatformPluginImpl>(env, plugin, kPluginPlatformSdl);
    } else if (kEnablePluginPlatformRef && strcmp(name, kPluginPlatformRef) == 0) {
        return PlatformPluginInit<ls::RefPlatformPluginImpl>(env, plugin, kPluginPlatformRef);
    } else if (kEnablePluginAudioSdlAudio && strcmp(name, kPluginAudioSdlAudio) == 0) {
        EnsureSDL2(env);
        return ls::AudioPluginInit<ls::SDLAudioPluginImpl>(env, plugin, kPluginAudioSdlAudio);
    } else if (kEnablePluginAudioSdlMixer && strcmp(name, kPluginAudioSdlMixer) == 0) {
        EnsureSDL2(env);
        EnsureSDL2_mixer(env);
        return ls::AudioPluginInit<ls::SDLMixerAudioPluginImpl>(env, plugin, kPluginAudioSdlMixer);
    } else if (kEnablePluginAudioRef && strcmp(name, kPluginAudioRef) == 0) {
        return ls::AudioPluginInit<ls::RefAudioPluginImpl>(env, plugin, kPluginAudioRef);
    } else {
        throw Napi::Error::New(env, "Unknown plugin name");
    }
}

static bool LoadFramework(const char* framework, const char* frameworkLib) {
    std17::filesystem::path frameworkLibPath(GetEnvOrDefault("LS_RUNTIME_FRAMEWORK_PATH", "/Library/Frameworks"));

    frameworkLibPath.append(framework).append(frameworkLib);

    try {
        SDL2::Open(frameworkLibPath.c_str());
        return true;
    } catch (std::exception& e) {
        // ignore
        return false;
    }
}

static void EnsureSDL2(const Napi::Env& env) {
    if (SDL2::IsOpen()) {
        return;
    }

    if (kIsMac && !HasEnv("LS_SDL_USE_DYLIB") && LoadFramework(kSDLFramework, kSDLFrameworkLib)) {
        return;
    }

    NAPI_TRY(env, SDL2::Open(GetEnvOrDefault("LS_SDL_LIB_NAME", kSDLDefaultLibName)));
}

static void EnsureSDL2_mixer(const Napi::Env& env) {
    if (SDL2::mixer::IsOpen()) {
        return;
    }

    if (kIsMac && !HasEnv("LS_SDL_MIXER_USE_DYLIB") && LoadFramework(kSDLMixerFramework, kSDLMixerFrameworkLib)) {
        return;
    }

    NAPI_TRY(env, SDL2::mixer::Open(GetEnvOrDefault("LS_SDL_MIXER_LIB_NAME", kSDLMixerDefaultLibName)));
}

} // namespace bindings
} // namespace ls
