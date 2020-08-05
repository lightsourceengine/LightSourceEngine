/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/AudioPlugin.h>
#include <ls/SDLAudioPluginImpl.h>

using Napi::Env;
using Napi::Object;

Object Init(Env env, Object exports) {
    return ls::AudioPluginInit<ls::SDLAudioPluginImpl>(env, exports, "light-source-sdl-audio");
}

NODE_API_MODULE(LightSourceSdlAudio, Init);
