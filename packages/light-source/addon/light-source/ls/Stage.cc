/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Stage.h"
#include <ls/Format.h>
#include <ls/Log.h>

using Napi::Array;
using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Object;
using Napi::SafeObjectWrap;
using Napi::String;
using Napi::SymbolFor;
using Napi::Value;

namespace ls {

Stage::Stage(const CallbackInfo& info) : SafeObjectWrap<Stage>(info) {
}

void Stage::Constructor(const Napi::CallbackInfo& info) {
}

Function Stage::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "StageBase", true, {
            InstanceMethod(SymbolFor(env, "destroy"), &Stage::Destroy),
        });
    }

    return constructor.Value();
}

void Stage::Destroy(const CallbackInfo& info) {
    this->threadPool.ShutdownNow();
}

} // namespace ls
