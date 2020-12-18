/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

#if defined(_WIN32)
constexpr const char* kSDLMixerDefaultLibName = "SDL2_mixer.dll";
#elif defined(__linux__)
constexpr const char* kSDLMixerDefaultLibName = "libSDL2_mixer-2.0.so.0";
#elif defined(__APPLE__)
constexpr const char* kSDLMixerDefaultLibName = "libSDL2_mixer.dylib";
#else
static_assert(false, "unknown platform");
#endif

constexpr const char* kSDLMixerFramework = "SDL2_mixer.framework";
constexpr const char* kSDLMixerFrameworkLib = "SDL2_mixer";

#define DYNAMIC_FOR_EACH_MIX_FUNCTION(APPLY)                \
    APPLY(Mix_Init)                                         \
    APPLY(Mix_Quit)                                         \
    APPLY(Mix_OpenAudio)                                    \
    APPLY(Mix_CloseAudio)                                   \
    APPLY(Mix_GetMusicDecoder)                              \
    APPLY(Mix_GetNumMusicDecoders)                          \
    APPLY(Mix_VolumeMusic)                                  \
    APPLY(Mix_PauseMusic)                                   \
    APPLY(Mix_ResumeMusic)                                  \
    APPLY(Mix_HaltMusic)                                    \
    APPLY(Mix_FadeOutMusic)                                 \
    APPLY(Mix_FadeInMusic)                                  \
    APPLY(Mix_PlayMusic)                                    \
    APPLY(Mix_FreeMusic)                                    \
    APPLY(Mix_LoadMUS_RW)                                   \
    APPLY(Mix_Volume)                                       \
    APPLY(Mix_Pause)                                        \
    APPLY(Mix_Resume)                                       \
    APPLY(Mix_LoadWAV_RW)                                   \
    APPLY(Mix_VolumeChunk)                                  \
    APPLY(Mix_FadeInChannelTimed)                           \
    APPLY(Mix_PlayChannelTimed)                             \
    APPLY(Mix_FadeOutChannel)                               \
    APPLY(Mix_HaltChannel)                                  \
    APPLY(Mix_FreeChunk)                                    \
    APPLY(Mix_GetNumChunkDecoders)                          \
    APPLY(Mix_GetChunkDecoder)

// Load SDL_mixer functions manually.
//
// The manual load is done for two reasons:
// 1) Light Source Engine developers do not have to worry about linking to SDL_mixer at compile time.
// 2) light-source.node can be loaded without SDL_mixer dependencies. If SDL_mixer is unavailable at runtime, the
//    javascript environment can do error reporting.

namespace ls {
namespace SDL2 {
namespace mixer {

#define DYNAMIC_MIX_DECLARE_FUNCTION_PTR(NAME) extern decltype(&::NAME) NAME;

DYNAMIC_FOR_EACH_MIX_FUNCTION(DYNAMIC_MIX_DECLARE_FUNCTION_PTR)

#undef DYNAMIC_MIX_DECLARE_FUNCTION_PTR

void Open(const char* library);
bool IsOpen() noexcept;
void Close() noexcept;

} // namespace mixer
} // namespace SDL2
} // namespace ls
