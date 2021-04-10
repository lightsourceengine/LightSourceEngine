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

#include "SDLAudioPlugin.h"
#include <lse/SDL2.h>
#include <lse/string-ext.h>

namespace lse {

static Uint16 ToSDLAudioFormat(const std::string& audioFormat) noexcept;

SDLAudioPlugin::SDLAudioPlugin(const AudioPluginConfig& config) : AudioPlugin(config) {
}

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

  desired.freq = this->config.frequency;
  desired.format = ToSDLAudioFormat(this->config.audioFormat);
  desired.channels = this->config.channels;
  desired.samples = this->config.bytesPerSample;

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

static Uint16 ToSDLAudioFormat(const std::string& audioFormat) noexcept {
  if (audioFormat.empty()) {
    // fallthrough to default
  } else if (EqualsIgnoreCase(audioFormat, "U16LSB")) {
    return AUDIO_U16LSB;
  } else if (EqualsIgnoreCase(audioFormat, "S16LSB")) {
    return AUDIO_S16LSB;
  } else if (EqualsIgnoreCase(audioFormat, "U16MSB")) {
    return AUDIO_U16MSB;
  } else if (EqualsIgnoreCase(audioFormat, "S16MSB")) {
    return AUDIO_S16MSB;
  } else if (EqualsIgnoreCase(audioFormat, "S32LSB")) {
    return AUDIO_S32LSB;
  } else if (EqualsIgnoreCase(audioFormat, "S32MSB")) {
    return AUDIO_S32MSB;
  } else if (EqualsIgnoreCase(audioFormat, "F32LSB")) {
    return AUDIO_F32LSB;
  } else if (EqualsIgnoreCase(audioFormat, "F32MSB")) {
    return AUDIO_F32MSB;
  } else if (EqualsIgnoreCase(audioFormat, "S8")) {
    return AUDIO_S8;
  } else if (EqualsIgnoreCase(audioFormat, "U8")) {
    return AUDIO_U8;
  } else if (EqualsIgnoreCase(audioFormat, "U16SYS")) {
    return AUDIO_U16SYS;
  } else if (EqualsIgnoreCase(audioFormat, "S16SYS")) {
    return AUDIO_S16SYS;
  } else if (EqualsIgnoreCase(audioFormat, "S32SYS")) {
    return AUDIO_S32SYS;
  } else if (EqualsIgnoreCase(audioFormat, "F32SYS")) {
    return AUDIO_F32SYS;
  }

  return AUDIO_U16SYS;
}

} // namespace lse
