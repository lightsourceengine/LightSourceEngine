/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include "SDLMixerAudioAdapter.h"

using Napi::Env;
using Napi::HandleScope;
using Napi::Object;
using ls::SDLMixerAudioAdapter;

Object Init(Env env, Object exports) {
    HandleScope scope(env);

    exports["AudioAdapter"] = SDLMixerAudioAdapter::GetClass(env);

    return exports;
}

NODE_API_MODULE(LightSourceSdlMixer, Init);
