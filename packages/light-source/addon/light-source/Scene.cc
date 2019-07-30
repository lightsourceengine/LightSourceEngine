/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Scene.h"
#include "BoxSceneNode.h"
#include "napi-ext.h"
#include <StageAdapter.h>
#include <fmt/format.h>

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Env;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::Reference;
using Napi::String;
using Napi::Value;

namespace ls {

Scene::Scene(const CallbackInfo& info) : ObjectWrap<Scene>(info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    this->resourceManager = ResourceManager::Unwrap(ResourceManager::Constructor(env).New({}));
    this->resourceManager->Ref();

    auto node{ BoxSceneNode::Constructor(env).New({ this->Value() }) };

    this->root = ObjectWrap<SceneNode>::Unwrap(node);
    this->root->AsReference()->Ref();

    auto jsThis{ info.This().As<Object>() };

    jsThis.Set("stage", info[0].As<Object>());
    jsThis.Set("root", node);
}

Function Scene::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "SceneBase", {
            InstanceValue("root", env.Null(), napi_writable),
            InstanceValue("stage", env.Null(), napi_writable),
            InstanceAccessor("resource", &Scene::GetResourceManager, nullptr),
            InstanceMethod("resize", &Scene::Resize),
            InstanceMethod(SymbolFor(env, "attach"), &Scene::Attach),
            InstanceMethod(SymbolFor(env, "detach"), &Scene::Detach),
            InstanceMethod(SymbolFor(env, "processEvents"), &Scene::ProcessEvents),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value Scene::GetResourceManager(const CallbackInfo& info) {
    return this->resourceManager->Value();
}

void Scene::Attach(const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);
    auto stage{ info.This().As<Object>().Get("stage").As<Object>() };
    auto stageAdapter{ ObjectWrap<StageAdapter>::Unwrap(stage.Get(SymbolFor(env, "adapter")).As<Object>()) };

    this->adapter = stageAdapter->CreateSceneAdapter(0);
    this->adapter->Attach();

    this->resourceManager->Attach(this->adapter->GetRenderer());
}

void Scene::Detach(const CallbackInfo& info) {
    this->adapter->Detach();
}

void Scene::Resize(const CallbackInfo& info) {
    this->adapter->Resize(
        info[0].As<Number>().Int32Value(),
        info[1].As<Number>().Int32Value(),
        info[2].As<Boolean>());
}

void Scene::ProcessEvents(const CallbackInfo& info) {
    this->resourceManager->ProcessEvents();
}

} // namespace ls
