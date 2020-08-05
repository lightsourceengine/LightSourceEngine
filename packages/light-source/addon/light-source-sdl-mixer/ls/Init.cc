/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/AudioPlugin.h>
#include <ls/SDLMixerAudioPluginImpl.h>

using Napi::Env;
using Napi::Object;

Object Init(Env env, Object exports) {
    return ls::AudioPluginInit<ls::SDLMixerAudioPluginImpl>(env, exports, "light-source-sdl-mixer");
}

NODE_API_MODULE(LightSourceSdlMixer, Init);
