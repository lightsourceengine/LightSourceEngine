/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

namespace lse {

constexpr bool kEnablePluginPlatformSdl
#if defined(LSE_ENABLE_PLUGIN_PLATFORM_SDL)
    = true;
#else
= false;
#endif

constexpr bool kEnablePluginAudioSdlAudio
#if defined(LSE_ENABLE_PLUGIN_AUDIO_SDL_AUDIO)
    = true;
#else
= false;
#endif

constexpr bool kEnablePluginAudioSdlMixer
#if defined(LSE_ENABLE_PLUGIN_AUDIO_SDL_MIXER)
    = true;
#else
= false;
#endif

constexpr const char* kDefaultRuntimeFrameworkPath = "/Library/Frameworks";

} // namespace lse
