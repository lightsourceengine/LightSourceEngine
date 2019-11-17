/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Stage.h"
#include "FontStore.h"
#include <ls/StageAdapter.h>
#include <ls/Log.h>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Object;
using Napi::SafeObjectWrap;
using Napi::QueryInterface;
using Napi::String;
using Napi::SymbolFor;
using Napi::Value;

namespace ls {

Stage::Stage(const CallbackInfo& info) : SafeObjectWrap<Stage>(info) {
}

void Stage::Constructor(const Napi::CallbackInfo& info) {
    this->fontStore.Attach(this);
    // TODO: need to detach somewhere
    this->taskQueue.Init(this->GetExecutor());
}

Function Stage::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "StageBase", {
            InstanceAccessor(SymbolFor(env, "adapter"), &Stage::GetStageAdapter, &Stage::SetStageAdapter),
            InstanceAccessor(SymbolFor(env, "resourcePath"), &Stage::GetResourcePath, &Stage::SetResourcePath),
            InstanceMethod(SymbolFor(env, "processEvents"), &Stage::ProcessEvents),
        });
    }

    return constructor.Value();
}

void Stage::Destroy() noexcept {
    if (this->stageAdapter) {
        try {
            this->stageAdapter->Unref();
        } catch (const Error& e) {
            LOG_ERROR(e);
        }

        this->stageAdapter = nullptr;
    }

    this->fontStore.Detach();
    this->executor.ShutdownNow();
    this->taskQueue.ShutdownNow();
}

void Stage::ProcessEvents(const CallbackInfo& info) {
    this->taskQueue.ProcessTasks();
}

Value Stage::GetStageAdapter(const CallbackInfo& info) {
    if (this->stageAdapter) {
        return this->stageAdapter->Value();
    }

    return info.Env().Null();
}

void Stage::SetStageAdapter(const CallbackInfo& info, const Napi::Value& value) {
    auto env{ info.Env() };
    auto oldStageAdapter{ this->stageAdapter };
    StageAdapter* newStageAdapter;

    if (value.IsObject()) {
        newStageAdapter = QueryInterface<StageAdapter>(value);
    } else if (value.IsNull() || value.IsUndefined()) {
        newStageAdapter = nullptr;
    } else {
        throw Error::New(env, "Invalid StageAdapter.");
    }

    if (oldStageAdapter == newStageAdapter) {
        return;
    }

    if (newStageAdapter) {
        newStageAdapter->Ref();
    }

    if (oldStageAdapter) {
        oldStageAdapter->Unref();
    }

    this->stageAdapter = newStageAdapter;
}

Value Stage::GetResourcePath(const CallbackInfo& info) {
    return String::New(info.Env(), this->resourcePath);
}

void Stage::SetResourcePath(const CallbackInfo& info, const Napi::Value& value) {
    if (value.IsNull() || value.IsUndefined()) {
        this->resourcePath = "";
    } else if (value.IsString()) {
        this->resourcePath = value.As<String>();
    } else {
        throw Error::New(info.Env(), "resourcePath must be a string.");
    }
}

} // namespace ls
