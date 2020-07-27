/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include "SDLAudioAdapter.h"

using Napi::Env;
using Napi::HandleScope;
using Napi::Object;
using ls::SDLAudioAdapter;

Object Init(Env env, Object) {
    return SDLAudioAdapter::GetClass(env);
}

NODE_API_MODULE(LightSourceSdlAudio, Init);
