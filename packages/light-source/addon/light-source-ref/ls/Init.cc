/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include "RefStageAdapter.h"
#include "RefAudioAdapter.h"

using Napi::Env;
using Napi::HandleScope;
using Napi::Object;
using ls::RefStageAdapter;
using ls::RefAudioAdapter;

Object Init(Env env, Object exports) {
    HandleScope scope(env);

    exports["StageAdapter"] = RefStageAdapter::GetClass(env);
    exports["AudioAdapter"] = RefAudioAdapter::GetClass(env);

    return exports;
}

NODE_API_MODULE(LightSourceRef, Init);
