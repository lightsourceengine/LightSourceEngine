/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Scene.h"
#include "RootSceneNode.h"
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

    auto stageAdapter{ ObjectWrap<StageAdapter>::Unwrap(info[0].As<Object>()) };

    this->adapter = stageAdapter->CreateSceneAdapter({
        info[1].As<Number>().Int32Value(),
        info[2].As<Number>().Int32Value(),
        info[3].As<Number>().Int32Value(),
        info[4].As<Boolean>().Value(),
    });

    auto resourceManagerValue{ ResourceManager::Constructor(env).New({}) };

    this->resourceManager = ResourceManager::Unwrap(resourceManagerValue);
    this->resourceManager->SetRenderer(this->adapter->GetRenderer());
    this->resourceManager->Ref();

    auto rootValue{ RootSceneNode::Constructor(env).New({ this->Value() }) };

    this->root = ObjectWrap<SceneNode>::Unwrap(rootValue);
    this->root->AsReference()->Ref();

    auto self{ info.This().As<Object>() };

    self.Set(SymbolFor(env, "root"), rootValue);
    self.Set(SymbolFor(env, "resource"), resourceManagerValue);
}

Function Scene::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "SceneBase", {
            InstanceValue(SymbolFor(env, "width"), Number::New(env, 0), napi_writable),
            InstanceValue(SymbolFor(env, "height"), Number::New(env, 0), napi_writable),
            InstanceValue(SymbolFor(env, "fullscreen"), Boolean::New(env, true), napi_writable),
            InstanceValue(SymbolFor(env, "root"), env.Null(), napi_writable),
            InstanceValue(SymbolFor(env, "resource"), env.Null(), napi_writable),
            InstanceAccessor("title", &Scene::GetTitle, &Scene::SetTitle),
            InstanceMethod("resize", &Scene::Resize),
            InstanceMethod(SymbolFor(env, "attach"), &Scene::Attach),
            InstanceMethod(SymbolFor(env, "detach"), &Scene::Detach),
            InstanceMethod(SymbolFor(env, "destroy"), &Scene::Destroy),
            InstanceMethod(SymbolFor(env, "frame"), &Scene::Frame),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void Scene::Attach(const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    // TODO: exceptions?
    this->adapter->Attach();
    this->resourceManager->Attach();

    auto self{ info.This().As<Object>() };

    this->width = this->adapter->GetWidth();
    this->height = this->adapter->GetHeight();

    self.Set(SymbolFor(env, "width"), Number::New(env, this->width));
    self.Set(SymbolFor(env, "height"), Number::New(env, this->height));
    self.Set(SymbolFor(env, "fullscreen"), Boolean::New(env, this->adapter->GetFullscreen()));
}

void Scene::Detach(const CallbackInfo& info) {
    if (this->resourceManager) {
        this->resourceManager->Detach();
    }

    if (this->adapter) {
        this->adapter->Detach();
    }
}

void Scene::Destroy(const CallbackInfo& info) {
    if (this->root) {
        this->root->Destroy();
        this->root->AsReference()->Unref();
        this->root = nullptr;
    }

    if (this->resourceManager) {
        this->resourceManager->Destroy();
        this->resourceManager->Unref();
        this->resourceManager = nullptr;
    }

    this->adapter.reset();
}

void Scene::Resize(const CallbackInfo& info) {
    this->adapter->Resize(
        info[0].As<Number>().Int32Value(),
        info[1].As<Number>().Int32Value(),
        info[2].As<Boolean>());
    this->recalculateLayoutRequested = true;
}

void Scene::Frame(const CallbackInfo& info) {
    auto renderer{ this->adapter->GetRenderer() };

    this->resourceManager->ProcessEvents();

    this->root->Layout(this->width, this->height, this->recalculateLayoutRequested);
    this->recalculateLayoutRequested = false;

    renderer->Reset();
    this->root->Paint(renderer);
    renderer->Present();
}

Value Scene::GetTitle(const CallbackInfo& info) {
    return String::New(info.Env(), this->adapter->GetTitle());
}

void Scene::SetTitle(const CallbackInfo& info, const Napi::Value& value) {
    if (value.IsString()) {
        this->adapter->SetTitle(value.As<String>());
    } else {
        this->adapter->SetTitle("");
    }
}

void Scene::NotifyRootFontSizeChanged(int32_t rootFontSize) {
    if (this->rootFontSize != rootFontSize) {
        this->rootFontSize = rootFontSize;
        this->recalculateLayoutRequested = true;
    }
}

} // namespace ls
