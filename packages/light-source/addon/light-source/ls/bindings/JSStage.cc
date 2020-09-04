/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/bindings/JSStage.h>

#include <ls/Stage.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;

static FunctionReference jsStageConstructor;

namespace ls {
namespace bindings {

void JSStage::Constructor(const CallbackInfo& info) {
    this->native = std::make_shared<Stage>();
}

Function JSStage::GetClass(Napi::Env env) {
    if (jsStageConstructor.IsEmpty()) {
        HandleScope scope(env);

        jsStageConstructor = DefineClass(env, "StageBase", true, {
            InstanceMethod("$destroy", &JSStage::Destroy),
        });
    }

    return jsStageConstructor.Value();
}

StageRef JSStage::GetNative() const noexcept {
    return this->native;
}

void JSStage::Destroy(const CallbackInfo& info) {
    this->native->Destroy();
}

} // namespace bindings
} // namespace ls
