/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/ResourceProgress.h>
#include <ls/Resources.h>
#include <ls/SceneNode.h>

using Napi::AssignFunctionReference;
using Napi::Call;
using Napi::Env;
using Napi::Error;
using Napi::HandleScope;

namespace ls {

void ResourceProgress::Dispatch(SceneNode* node, Res* resource) const {
    switch (resource->GetState()) {
        case Res::Ready:
            if (!this->onLoad.IsEmpty()) {
                Env env(node->Env());
                HandleScope scope(env);

                try {
                    Call(this->onLoad, { node->Value(), resource->Summarize(env) });
                } catch (std::exception& e) {
                    LOG_WARN("onLoad unhandled exception: %s", e);
                }
            }
            break;
        case Res::Error:
            if (!this->onError.IsEmpty()) {
                Env env(node->Env());
                HandleScope scope(env);

                try {
                    this->onError.Call({ node->Value(), resource->GetErrorMessage(env)});
                } catch (std::exception& e) {
                    LOG_WARN("onError unhandled exception: %s", e);
                }
            }
            break;
        default:
            break;
    }
}

void ResourceProgress::Reset() {
    this->onLoad.Reset();
    this->onError.Reset();
}

void ResourceProgress::SetOnLoad(const Napi::Env& env, const Napi::Value& value) {
    if (!AssignFunctionReference(this->onLoad, value)) {
        throw Error::New(env, "onLoad invalid assignment.");
    }
}

Napi::Value ResourceProgress::GetOnLoad(const Napi::Env& env) const {
    return this->onLoad.IsEmpty() ? env.Null() : this->onLoad.Value();
}

void ResourceProgress::SetOnError(const Napi::Env& env, const Napi::Value& value) {
    if (!AssignFunctionReference(this->onError, value)) {
        throw Error::New(env, "onLoad invalid assignment.");
    }
}

Napi::Value ResourceProgress::GetOnError(const Napi::Env& env) const {
    return this->onError.IsEmpty() ? env.Null() : this->onError.Value();
}

} // namespace ls
