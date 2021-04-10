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

#include "SDLMixerAudioDestination.h"

#include <lse/AudioUtil.h>

namespace lse {

void SDLMixerSample::Destroy() {
}

std::vector<std::string> SDLMixerSample::GetDecoders() {
  return ListDecoders(SDL2::mixer::Mix_GetNumChunkDecoders(),
    [](int32_t i) { return SDL2::mixer::Mix_GetChunkDecoder(i); });
}

void SDLMixerSample::Resume() {
  SDL2::mixer::Mix_Resume(ALL_CHANNELS);
}

void SDLMixerSample::Pause() {
  SDL2::mixer::Mix_Pause(ALL_CHANNELS);
}

void SDLMixerSample::Stop(int32_t fadeOutMs) {
  if (fadeOutMs > 0) {
    SDL2::mixer::Mix_FadeOutChannel(ALL_CHANNELS, fadeOutMs);
  } else {
    SDL2::mixer::Mix_HaltChannel(ALL_CHANNELS);
  }
}

float SDLMixerSample::GetVolume() {
  return ConstrainVolume(static_cast<float>(SDL2::mixer::Mix_Volume(ALL_CHANNELS, -1)) / MIX_MAX_VOLUME_F);
}

void SDLMixerSample::SetVolume(float volume) {
  SDL2::mixer::Mix_Volume(ALL_CHANNELS, static_cast<int32_t>(ConstrainVolume(volume) * MIX_MAX_VOLUME_F));
}

bool SDLMixerSample::HasCapability(AudioDestinationCapability capability) {
  return true;
}

void SDLMixerStream::Destroy() {
}

std::vector<std::string> SDLMixerStream::GetDecoders() {
  return ListDecoders(SDL2::mixer::Mix_GetNumMusicDecoders(),
    [](int32_t i) { return SDL2::mixer::Mix_GetMusicDecoder(i); });
}

void SDLMixerStream::Resume() {
  SDL2::mixer::Mix_ResumeMusic();
}

void SDLMixerStream::Pause() {
  SDL2::mixer::Mix_PauseMusic();
}

void SDLMixerStream::Stop(int32_t fadeOutMs) {
  if (fadeOutMs > 0) {
    SDL2::mixer::Mix_FadeOutMusic(fadeOutMs);
  } else {
    SDL2::mixer::Mix_HaltMusic();
  }
}

float SDLMixerStream::GetVolume() {
  return ConstrainVolume(static_cast<float>(SDL2::mixer::Mix_VolumeMusic(-1)) / MIX_MAX_VOLUME_F);
}

void SDLMixerStream::SetVolume(float volume) {
  SDL2::mixer::Mix_VolumeMusic(static_cast<int32_t>(ConstrainVolume(volume) * MIX_MAX_VOLUME_F));
}

bool SDLMixerStream::HasCapability(AudioDestinationCapability capability) {
  return true;
}

} // namespace lse
