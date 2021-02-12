/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLAudioPlugin.h"
#include <lse/SDL2.h>
#include <lse/string-ext.h>

namespace lse {

void SDLAudioPlugin::Attach() {
  if (this->isAttached) {
    return;
  }

  // Initialize SDL Audio.
  if (SDL2::SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL2::SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    throw std::runtime_error(Format("Failed to init SDL audio. SDL Error: %s", SDL2::SDL_GetError()));
  }

  // Open an audio device.
  SDL_AudioSpec desired{};

  desired.freq = 44100;
  desired.format = AUDIO_S16SYS;
  desired.channels = 2;
  desired.samples = 512;

  if (SDL2::SDL_OpenAudio(&desired, nullptr) != 0) {
    throw std::runtime_error(Format("Cannot open audio. SDL_OpenAudio: %s", SDL2::SDL_GetError()));
  }

  // Ensure the device is silent.
  SDL2::SDL_PauseAudio(0);

  this->isAttached = true;
}

void SDLAudioPlugin::Detach() {
  if (!this->isAttached) {
    return;
  }

  SDL2::SDL_CloseAudio();
  SDL2::SDL_QuitSubSystem(SDL_INIT_AUDIO);

  this->isAttached = false;
}

void SDLAudioPlugin::Destroy() {
  this->Detach();
}

std::vector<std::string> SDLAudioPlugin::GetDevices() const noexcept {
  auto len{ SDL2::SDL_GetNumAudioDevices(0) };
  std::vector<std::string> audioDeviceNames;

  audioDeviceNames.reserve(len);

  for (auto i = 0; i < len; i++) {
    audioDeviceNames.emplace_back(SDL2::SDL_GetAudioDeviceName(i, 0));
  }

  return audioDeviceNames;
}

} // namespace lse
