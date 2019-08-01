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

const auto DEFAULT_ROOT_FONT_SIZE{ 16 };

Scene::Scene(const CallbackInfo& info) : ObjectWrap<Scene>(info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    auto stageAdapter{ ObjectWrap<StageAdapter>::Unwrap(info[0].As<Object>()) };

    this->adapter = stageAdapter->CreateSceneAdapter();

    auto resourceManagerValue{ ResourceManager::Constructor(env).New({}) };

    this->resourceManager = ResourceManager::Unwrap(resourceManagerValue);
    this->resourceManager->SetRenderer(this->adapter->GetRenderer());
    this->resourceManager->Ref();

    auto rootValue{ BoxSceneNode::Constructor(env).New({ this->Value() }) };

    this->root = ObjectWrap<SceneNode>::Unwrap(rootValue);
    this->root->AsReference()->Ref();

    auto self{ info.This().As<Object>() };

    self.Set(SymbolFor(env, "root"), rootValue);
    self.Set(SymbolFor(env, "resource"), resourceManagerValue);

    this->rootFontSize = DEFAULT_ROOT_FONT_SIZE;
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

    this->adapter->Attach();
    this->resourceManager->SetRenderer(this->adapter->GetRenderer());

    auto self{ info.This().As<Object>() };

    this->width = this->adapter->GetWidth();
    this->height = this->adapter->GetHeight();

    self.Set(SymbolFor(env, "width"), Number::New(env, this->width));
    self.Set(SymbolFor(env, "height"), Number::New(env, this->height));
    self.Set(SymbolFor(env, "fullscreen"), Boolean::New(env, this->adapter->GetFullscreen()));
}

void Scene::Detach(const CallbackInfo& info) {
    this->adapter->Detach();
}

void Scene::Resize(const CallbackInfo& info) {
    this->adapter->Resize(
        info[0].As<Number>().Int32Value(),
        info[1].As<Number>().Int32Value(),
        info[2].As<Boolean>());
    this->shouldRecalculateLayout = true;
}

void Scene::Frame(const CallbackInfo& info) {
    auto renderer{ this->adapter->GetRenderer() };

    this->resourceManager->ProcessEvents();

    // TODO: root node should generate an event when font size changes
    UpdateRootFontSize();

    this->root->Layout(this->width, this->height, this->recalculateLayoutRequested);
    this->recalculateLayoutRequested = false;

    renderer->Reset();
    this->root->Paint(renderer);
    renderer->Present();
}

Value Scene::GetTitle(const CallbackInfo& info) {
    return String::New(info.Env(), "");
}

void Scene::SetTitle(const CallbackInfo& info, const Napi::Value& value) {
}

void Scene::UpdateRootFontSize() {
    auto rootStyleFontSize{ this->root->GetStyleOrEmpty()->fontSize() };
    auto computedFontSize{ 0 };

    if (rootStyleFontSize) {
        switch (rootStyleFontSize->GetUnit()) {
            case StyleNumberUnitPoint:
                computedFontSize = rootStyleFontSize->GetValue();
                break;
            case StyleNumberUnitViewportWidth:
                computedFontSize = rootStyleFontSize->GetValuePercent() * this->GetWidth();
                break;
            case StyleNumberUnitViewportHeight:
                computedFontSize = rootStyleFontSize->GetValuePercent() * this->GetHeight();
                break;
            case StyleNumberUnitViewportMin:
                computedFontSize = rootStyleFontSize->GetValuePercent() * this->GetViewportMax();
                break;
            case StyleNumberUnitViewportMax:
                computedFontSize = rootStyleFontSize->GetValuePercent() * this->GetViewportMin();
                break;
            case StyleNumberUnitRootEm:
                computedFontSize = rootStyleFontSize->GetValue() * DEFAULT_ROOT_FONT_SIZE;
                break;
            default:
                break;
        }
    }

    if (this->rootFontSize != computedFontSize) {
        this->rootFontSize = computedFontSize;

        return this->shouldRecalculateLayout = true;
    }
}

} // namespace ls
