/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>

#include <ls/AudioPlugin.h>
#include <ls/RefAudioPluginImpl.h>

using Napi::Env;
using Napi::Object;

Object Init(Env env, Object exports) {
    return ls::AudioPluginInit<ls::RefAudioPluginImpl>(env, exports, "light-source-ref-audio");
}

NODE_API_MODULE(LightSourceRefAudio, Init);
