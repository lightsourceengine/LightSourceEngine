/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <SDL.h>

#if defined(_WIN32)
constexpr const char* kSDLDefaultLibName = "SDL2.dll";
#elif defined(__linux__)
constexpr const char* kSDLDefaultLibName = "libSDL2-2.0.so.0";
#elif defined(__APPLE__)
constexpr const char* kSDLDefaultLibName = "libSDL2.dylib";
#else
static_assert(false, "unknown platform");
#endif

constexpr const char* kSDLFramework = "SDL2.framework";
constexpr const char* kSDLFrameworkLib = "SDL2";

#define DYNAMIC_FOR_EACH_SDL_FUNCTION(APPLY)                \
    APPLY(SDL_Init)                                         \
    APPLY(SDL_InitSubSystem)                                \
    APPLY(SDL_Quit)                                         \
    APPLY(SDL_QuitSubSystem)                                \
    APPLY(SDL_WasInit)                                      \
    APPLY(SDL_CreateTexture)                                \
    APPLY(SDL_DestroyTexture)                               \
    APPLY(SDL_SetTextureBlendMode)                          \
    APPLY(SDL_SetTextureColorMod)                           \
    APPLY(SDL_SetTextureAlphaMod)                           \
    APPLY(SDL_LockTexture)                                  \
    APPLY(SDL_UnlockTexture)                                \
    APPLY(SDL_QueryTexture)                                 \
    APPLY(SDL_UpdateTexture)                                \
    APPLY(SDL_GetPixelFormatName)                           \
    APPLY(SDL_GetVideoDriver)                               \
    APPLY(SDL_GetNumVideoDrivers)                           \
    APPLY(SDL_GetCurrentVideoDriver)                        \
    APPLY(SDL_GetRenderDriverInfo)                          \
    APPLY(SDL_GetRendererInfo)                              \
    APPLY(SDL_CreateRenderer)                               \
    APPLY(SDL_DestroyRenderer)                              \
    APPLY(SDL_GetRendererOutputSize)                        \
    APPLY(SDL_RenderCopy)                                   \
    APPLY(SDL_RenderCopyEx)                                 \
    APPLY(SDL_SetRenderDrawColor)                           \
    APPLY(SDL_SetRenderDrawBlendMode)                       \
    APPLY(SDL_SetRenderTarget)                              \
    APPLY(SDL_RenderClear)                                  \
    APPLY(SDL_RenderPresent)                                \
    APPLY(SDL_RenderSetClipRect)                            \
    APPLY(SDL_GetDesktopDisplayMode)                        \
    APPLY(SDL_GetDisplayName)                               \
    APPLY(SDL_GetDisplayMode)                               \
    APPLY(SDL_GetNumDisplayModes)                           \
    APPLY(SDL_GetNumVideoDisplays)                          \
    APPLY(SDL_GetVersion)                                   \
    APPLY(SDL_JoystickOpen)                                 \
    APPLY(SDL_JoystickClose)                                \
    APPLY(SDL_JoystickName)                                 \
    APPLY(SDL_JoystickInstanceID)                           \
    APPLY(SDL_JoystickGetButton)                            \
    APPLY(SDL_JoystickNumAxes)                              \
    APPLY(SDL_JoystickNumHats)                              \
    APPLY(SDL_JoystickNumButtons)                           \
    APPLY(SDL_JoystickGetGUID)                              \
    APPLY(SDL_NumJoysticks)                                 \
    APPLY(SDL_JoystickGetGUIDString)                        \
    APPLY(SDL_GameControllerEventState)                     \
    APPLY(SDL_GameControllerAddMappingsFromRW)              \
    APPLY(SDL_GameControllerMappingForGUID)                 \
    APPLY(SDL_GetKeyboardState)                             \
    APPLY(SDL_PeepEvents)                                   \
    APPLY(SDL_PumpEvents)                                   \
    APPLY(SDL_CreateWindow)                                 \
    APPLY(SDL_DestroyWindow)                                \
    APPLY(SDL_SetWindowTitle)                               \
    APPLY(SDL_GetWindowDisplayMode)                         \
    APPLY(SDL_GetWindowFlags)                               \
    APPLY(SDL_ShowCursor)                                   \
    APPLY(SDL_RWFromFile)                                   \
    APPLY(SDL_RWFromMem)                                    \
    APPLY(SDL_LoadWAV_RW)                                   \
    APPLY(SDL_FreeWAV)                                      \
    APPLY(SDL_OpenAudio)                                    \
    APPLY(SDL_CloseAudio)                                   \
    APPLY(SDL_PauseAudio)                                   \
    APPLY(SDL_QueueAudio)                                   \
    APPLY(SDL_ClearQueuedAudio)                             \
    APPLY(SDL_GetNumAudioDevices)                           \
    APPLY(SDL_GetAudioDeviceName)                           \
    APPLY(SDL_SetHint)                                      \
    APPLY(SDL_GetError)

// Load SDL2 functions manually.
//
// The manual load is done for two reasons:
// 1) Light Source Engine developers do not have to worry about linking to SDL2 at compile time.
// 2) lse-core.node can be loaded without SDL2 dependencies. If SDL2 is unavailable at runtime, the javascript
//    environment can do error reporting.

namespace lse {
namespace SDL2 {

#define DYNAMIC_SDL_DECLARE_FUNCTION_PTR(NAME) extern decltype(&::NAME) NAME;

DYNAMIC_FOR_EACH_SDL_FUNCTION(DYNAMIC_SDL_DECLARE_FUNCTION_PTR)

#undef DYNAMIC_SDL_DECLARE_FUNCTION_PTR

void Open(const char* library);
bool IsOpen() noexcept;
void Close() noexcept;

} // namespace SDL2
} // namespace lse
