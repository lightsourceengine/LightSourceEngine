/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Scene.h"
#include "RootSceneNode.h"
#include "Stage.h"
#include <ls/Renderer.h>
#include <ls/GraphicsContext.h>
#include <ls/Log.h>
#include <ls/Format.h>
#include <ls/Math.h>
#include <ls/Style.h>
#include <ls/RootSceneNode.h>

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
using Napi::SafeObjectWrap;
using Napi::QueryInterface;
using Napi::Reference;
using Napi::String;
using Napi::SymbolFor;
using Napi::Value;

namespace ls {

Scene::Scene(const CallbackInfo& info) : SafeObjectWrap<Scene>(info) {
}

void Scene::Constructor(const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    this->stage = QueryInterface<Stage>(info[0].As<Object>());
    this->stage->Ref();

    this->graphicsContext = QueryInterface<GraphicsContext>(info[1].As<Object>());
    this->graphicsContext->Ref();

    this->root = QueryInterface<SceneNode>(RootSceneNode::GetClass(env).New({ info.This() }));
    this->root->Ref();
}

Function Scene::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "SceneBase", true, {
            InstanceAccessor("root", &Scene::GetRoot, nullptr),
            InstanceAccessor("stage", &Scene::GetStage, nullptr),
            InstanceAccessor(SymbolFor(env, "adapter"), &Scene::GetAdapter, nullptr),
            InstanceMethod(SymbolFor(env, "attach"), &Scene::Attach),
            InstanceMethod(SymbolFor(env, "detach"), &Scene::Detach),
            InstanceMethod(SymbolFor(env, "destroy"), &Scene::Destroy),
            InstanceMethod(SymbolFor(env, "frame"), &Scene::Frame),
        });
    }

    return constructor.Value();
}

void Scene::Attach(const CallbackInfo& info) {
    this->width = this->graphicsContext->GetWidth();
    this->height = this->graphicsContext->GetHeight();
    this->isAttached = true;
}

void Scene::Detach(const CallbackInfo& info) {
    // TODO: exceptions?

    this->isAttached = false;
}

void Scene::Frame(const CallbackInfo& info) {
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

    this->paintContext.Reset(renderer);

    if (!this->paintRequests.empty()) {
        std::for_each(this->paintRequests.begin(), this->paintRequests.end(),
        [paintContext = &this->paintContext](SceneNode* node) {
            node->Paint(paintContext);
        });

        this->paintRequests.clear();
    }

    if (this->hasCompositeRequest) {
        renderer->SetRenderTarget(nullptr);

        this->compositeContext.Reset(renderer);
        this->root->Composite(&this->compositeContext);
        this->hasCompositeRequest = false;

        renderer->Present();
    }
}

Napi::Value Scene::GetRoot(const Napi::CallbackInfo& info) {
    return this->root->Value();
}

Napi::Value Scene::GetStage(const Napi::CallbackInfo& info) {
    return this->stage->Value();
}

Napi::Value Scene::GetAdapter(const Napi::CallbackInfo &info) {
    return this->graphicsContext->Value();
}

void Scene::Destroy(const Napi::CallbackInfo &info) {
    if (this->root) {
        this->root->Destroy();
        this->root->Unref();
        this->root = nullptr;
    }

    if (this->stage) {
        this->stage->Unref();
        this->stage = nullptr;
    }

    if (this->graphicsContext) {
        this->graphicsContext->Unref();
        this->graphicsContext = nullptr;
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
    return this->graphicsContext->GetRenderer();
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
