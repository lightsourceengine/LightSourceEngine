/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Scene.h"
#include "RootSceneNode.h"
#include "Stage.h"
#include <ls/Renderer.h>
#include <napi-ext.h>
#include <ls/StageAdapter.h>
#include <ls/SceneAdapter.h>
#include <ls/Log.h>
#include <ls/Math.h>
#include <ls/Style.h>
#include "CompositeContext.h"

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Env;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::Reference;
using Napi::String;
using Napi::SymbolFor;
using Napi::Value;

namespace ls {

Scene::Scene(const CallbackInfo& info) : ObjectWrap<Scene>(info) {
    try {
        this->Construct(info);
    } catch (const Error& e) {
        LOG_ERROR("%s", e.what());
        this->stage = nullptr;
    }
}

void Scene::Construct(const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    this->stage = Stage::Unwrap(info[0].As<Object>());

    this->adapter = stage->GetStageAdapter()->CreateSceneAdapter({
        info[1].As<Number>().Int32Value(),
        info[2].As<Number>().Int32Value(),
        info[3].As<Number>().Int32Value(),
        info[4].As<Boolean>().Value(),
    });

    try {
        this->stage->Ref();
    } catch (const Error& e) {
        this->adapter.reset();
        this->stage = nullptr;
    }
}

Function Scene::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "SceneBase", {
            InstanceValue(SymbolFor(env, "width"), Number::New(env, 0), napi_writable),
            InstanceValue(SymbolFor(env, "height"), Number::New(env, 0), napi_writable),
            InstanceValue(SymbolFor(env, "fullscreen"), Boolean::New(env, true), napi_writable),
            InstanceAccessor("stage", &Scene::GetStage, nullptr),
            InstanceAccessor(SymbolFor(env, "root"), &Scene::GetRoot, &Scene::SetRoot),
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
    if (this->isAttached) {
        return;
    }

    auto env{ info.Env() };
    HandleScope scope(env);

    // TODO: exceptions?
    this->adapter->Attach();
    this->imageStore.Attach(this);

    auto self{ info.This().As<Object>() };

    this->width = this->adapter->GetWidth();
    this->height = this->adapter->GetHeight();

    self.Set(SymbolFor(env, "width"), Number::New(env, this->width));
    self.Set(SymbolFor(env, "height"), Number::New(env, this->height));
    self.Set(SymbolFor(env, "fullscreen"), Boolean::New(env, this->adapter->GetFullscreen()));

    this->isAttached = true;
}

void Scene::Detach(const CallbackInfo& info) {
    if (!this->isAttached) {
        return;
    }

    // TODO: exceptions?

    this->imageStore.Detach();

    if (this->adapter) {
        this->adapter->Detach();
    }

    this->isAttached = false;
}

void Scene::Destroy(const CallbackInfo& info) {
    if (this->root) {
        this->root->Destroy();
        this->root->AsReference()->Unref();
        this->root = nullptr;
    }

    // TODO: image store destroy?

    this->adapter.reset();

    if (this->stage) {
        this->stage->Unref();
        this->stage = nullptr;
    }

    this->isAttached = false;
}

void Scene::Resize(const CallbackInfo& info) {
    this->adapter->Resize(
        info[0].As<Number>().Int32Value(),
        info[1].As<Number>().Int32Value(),
        info[2].As<Boolean>());
    this->isSizeDirty = true;
}

void Scene::Frame(const CallbackInfo& info) {
    this->imageStore.ProcessEvents();

    if (this->isSizeDirty || this->isRootFontSizeDirty) {
        SceneNode::Visit(this->root, [=](SceneNode* node) {
            if (node->style != nullptr) {
                node->style->UpdateDependentProperties(this->isRootFontSizeDirty, this->isSizeDirty);
            }
        });

        this->isSizeDirty = false;
        this->isRootFontSizeDirty = false;
    }

    if (!this->beforeLayoutRequests.empty()) {
        std::for_each(this->beforeLayoutRequests.begin(), this->beforeLayoutRequests.end(),
            [](SceneNode* node) {
                node->BeforeLayout();
            });

        this->beforeLayoutRequests.clear();
    }

    this->root->Layout(this->width, this->height);

    if (!this->afterLayoutRequests.empty()) {
        std::for_each(this->afterLayoutRequests.begin(), this->afterLayoutRequests.end(),
                      [](SceneNode* node) {
                          node->AfterLayout();
                      });

        this->afterLayoutRequests.clear();
    }

    if (!this->isAttached) {
        return;
    }

    auto renderer{ this->GetRenderer() };

    if (!this->paintRequests.empty()) {
        std::for_each(this->paintRequests.begin(), this->paintRequests.end(),
                      [renderer](SceneNode* node) {
                          node->Paint(renderer);
                      });

        this->paintRequests.clear();
    }

    if (this->hasCompositeRequest) {
        renderer->SetRenderTarget(nullptr);

        CompositeContext context;

        context.Reset(renderer);

        this->root->Composite(&context);

        this->hasCompositeRequest = false;

        renderer->Present();
    }
}

Value Scene::GetTitle(const CallbackInfo& info) {
    return String::New(info.Env(), this->adapter->GetTitle());
}

Value Scene::GetStage(const CallbackInfo& info) {
    return this->stage ? this->stage->Value() : info.Env().Null();
}

Napi::Value Scene::GetRoot(const Napi::CallbackInfo& info) {
    return this->root->AsReference()->Value();
}

void Scene::SetRoot(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (this->root) {
        this->root->AsReference()->Unref();
        this->root = nullptr;
    }

    if (value.IsObject()) {
        this->root = ObjectWrap<SceneNode>::Unwrap(value.As<Object>());

        if (this->root) {
            this->root->AsReference()->Ref();
        }
    }
}

void Scene::SetTitle(const CallbackInfo& info, const Napi::Value& value) {
    if (value.IsString()) {
        this->adapter->SetTitle(value.As<String>());
    } else {
        this->adapter->SetTitle("");
    }
}

void Scene::QueueRootFontSizeChange(float rootFontSize) {
    if (!Equals(this->rootFontSize, rootFontSize)) {
        this->rootFontSize = rootFontSize;

        if (!this->root->children.empty()) {
            this->isRootFontSizeDirty = true;
        }
    }
}

void Scene::SetActiveNode(Napi::Value node) {
    auto env{ this->Env() };
    HandleScope scope(env);
    auto self{ this->Value() };

    try {
        self["activeNode"] = node;
    } catch (const Error& e) {
        LOG_ERROR(e);
    }
}

void Scene::QueuePaint(SceneNode* node) {
    this->paintRequests.insert(node);
}

void Scene::QueueAfterLayout(SceneNode* node) {
    this->afterLayoutRequests.insert(node);
}

void Scene::QueueBeforeLayout(SceneNode* node) {
    this->beforeLayoutRequests.insert(node);
}

Renderer* Scene::GetRenderer() const noexcept {
    return this->adapter->GetRenderer();
}

void Scene::QueueComposite() {
    this->hasCompositeRequest = true;
}

void Scene::Remove(SceneNode* node) {
    this->paintRequests.erase(node);
    this->beforeLayoutRequests.erase(node);
    this->afterLayoutRequests.erase(node);
}

} // namespace ls
