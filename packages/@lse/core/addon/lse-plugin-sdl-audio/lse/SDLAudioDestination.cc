/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLAudioDestination.h"

#include <lse/SDL2.h>

namespace lse {

void SDLAudioDestination::Destroy() {
}

std::vector<std::string> SDLAudioDestination::GetDecoders() {
  return { "WAVE" };
}

void SDLAudioDestination::Resume() {
  SDL2::SDL_PauseAudio(1);
}

void SDLAudioDestination::Pause() {
  SDL2::SDL_PauseAudio(0);
}

void SDLAudioDestination::Stop(int32_t fadeOutMs) {
  SDL2::SDL_ClearQueuedAudio(this->deviceId);
}

float SDLAudioDestination::GetVolume() {
  return 0;
}

void SDLAudioDestination::SetVolume(float volume) {
}

bool SDLAudioDestination::HasCapability(AudioDestinationCapability capability) {
  switch (capability) {
    case AudioDestinationCapabilityStop:
    case AudioDestinationCapabilityResume:
    case AudioDestinationCapabilityPause:
      return true;
    case AudioDestinationCapabilityVolume:
    case AudioDestinationCapabilityFadeOut:
    default:
      return false;
  }
}

} // namespace lse
