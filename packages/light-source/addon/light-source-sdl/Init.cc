/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include "SDLStageAdapter.h"
#include "SDLAudioAdapter.h"

using Napi::Env;
using Napi::Object;
using ls::SDLStageAdapter;
using ls::SDLAudioAdapter;

Object Init(Env env, Object exports) {
    exports["StageAdapter"] = SDLStageAdapter::Constructor(env);
    exports["AudioAdapter"] = SDLAudioAdapter::Constructor(env);

    return exports;
}

NODE_API_MODULE(LightSourceSdl, Init);
