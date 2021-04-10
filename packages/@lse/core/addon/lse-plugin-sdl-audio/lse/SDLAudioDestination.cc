/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
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
