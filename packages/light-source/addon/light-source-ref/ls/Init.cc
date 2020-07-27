/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include "RefStageAdapter.h"

using Napi::Env;
using Napi::HandleScope;
using Napi::Object;
using ls::RefStageAdapter;

Object Init(Env env, Object exports) {
    return RefStageAdapter::GetClass(env);
}

NODE_API_MODULE(LightSourceRef, Init);
