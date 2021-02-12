/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLMixerAudioPlugin.h"

#include <lse/AudioUtil.h>
#include <lse/SDL2.h>
#include <lse/SDL2_mixer.h>
#include <lse/string-ext.h>

namespace lse {

void SDLMixerAudioPlugin::Attach() {
  if (this->isAttached) {
    return;
  }

  if (SDL2::SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL2::SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    throw std::runtime_error(Format("Failed to init SDL audio. SDL Error: %s", SDL2::SDL_GetError()));
  }

  if (SDL2::mixer::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 512) < 0) {
    throw std::runtime_error(Format("Cannot open mixer. Error: %s", SDL2::SDL_GetError()));
  }

  SDL2::mixer::Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG
      // MIX_INIT_OPUS and MIX_INIT_MID not available in 2.0.0 headers..it's harmless to pass
      // in the new values
      | /*MIX_INIT_MID*/ 0x00000020 | /*MIX_INIT_OPUS*/ 0x00000040);

  this->isAttached = true;
}

void SDLMixerAudioPlugin::Detach() {
  if (!this->isAttached) {
    return;
  }

  SDL2::mixer::Mix_CloseAudio();

  // Documentation recommended way to call Mix_Quit()
  while (SDL2::mixer::Mix_Init(0)) {
    SDL2::mixer::Mix_Quit();
  }

  SDL2::SDL_QuitSubSystem(SDL_INIT_AUDIO);

  this->isAttached = false;
}

void SDLMixerAudioPlugin::Destroy() {
  this->Detach();
}

std::vector<std::string> SDLMixerAudioPlugin::GetDevices() const noexcept {
  return ListDecoders(SDL2::SDL_GetNumAudioDevices(0),
    [](int32_t i){ return SDL2::SDL_GetAudioDeviceName(i, 0); });
}

} // namespace lse
