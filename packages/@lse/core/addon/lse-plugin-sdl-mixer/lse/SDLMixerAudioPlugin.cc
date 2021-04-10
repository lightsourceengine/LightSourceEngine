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

#include "SDLMixerAudioPlugin.h"

#include <lse/AudioUtil.h>
#include <lse/SDL2.h>
#include <lse/SDL2_mixer.h>
#include <lse/string-ext.h>

namespace lse {

static Uint16 ToSDLAudioFormat(const std::string& audioFormat) noexcept;
static void MixInitFromConfig(const std::vector<std::string>& decoders) noexcept;

SDLMixerAudioPlugin::SDLMixerAudioPlugin(const AudioPluginConfig& config) : AudioPlugin(config) {
}

void SDLMixerAudioPlugin::Attach() {
  if (this->isAttached) {
    return;
  }

  if (SDL2::SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL2::SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    throw std::runtime_error(Format("Failed to init SDL audio. SDL Error: %s", SDL2::SDL_GetError()));
  }

  if (SDL2::mixer::Mix_OpenAudio(
      this->config.frequency,
      ToSDLAudioFormat(this->config.audioFormat),
      this->config.channels,
      this->config.bytesPerSample) < 0) {
    throw std::runtime_error(Format("Cannot open mixer. Error: %s", SDL2::SDL_GetError()));
  }

  MixInitFromConfig(this->config.decoders);

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

static void MixInitFromConfig(const std::vector<std::string>& decoders) noexcept {
  // if no decoders specified, try mp3, if not available, try ogg.
  if (decoders.empty()) {
    if (SDL2::mixer::Mix_Init(MIX_INIT_MP3) == 0) {
        SDL2::mixer::Mix_Init(MIX_INIT_OGG);
    }
    return;
  }

  int32_t flags{};

  // note: MIX_INIT_OPUS and MIX_INIT_MID not available in 2.0.0 headers
  for (const auto& decoder : decoders) {
    if (EqualsIgnoreCase(decoder, "OGG")) {
      flags |= MIX_INIT_OGG;
    } else if (EqualsIgnoreCase(decoder, "MP3")) {
      flags |= MIX_INIT_MP3;
    } else if (EqualsIgnoreCase(decoder, "FLAC")) {
      flags |= MIX_INIT_FLAC;
    } else if (EqualsIgnoreCase(decoder, "MOD")) {
      flags |= MIX_INIT_MOD;
    } else if (EqualsIgnoreCase(decoder, "OPUS")) {
      flags |= /*MIX_INIT_OPUS*/ 0x00000040;
    } else if (EqualsIgnoreCase(decoder, "MID")) {
      flags |= /*MIX_INIT_MID*/ 0x00000020;
    }
  }

  SDL2::mixer::Mix_Init(flags);
}

} // namespace lse
