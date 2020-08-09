/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/Scene.h>

#include <ls/Stage.h>
#include <ls/Renderer.h>
#include <ls/GraphicsContext.h>
#include <ls/Log.h>
#include <ls/Format.h>
#include <ls/Math.h>
#include <ls/Style.h>
#include <ls/RootSceneNode.h>
#include <ls/StyleUtils.h>
#include <YGNode.h>

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
using Napi::Reference;
using Napi::String;
using Napi::SymbolFor;
using Napi::Value;

namespace ls {

void Scene::Constructor(const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    this->stage = Stage::Cast(info[0].As<Object>());
    this->stage->Ref();

    this->graphicsContext = GraphicsContext::Cast(info[1].As<Object>());
    this->graphicsContext->Ref();

    this->root = SceneNode::QueryInterface(RootSceneNode::GetClass(env).New({ info.This() }));
    this->root->Ref();
}

Function Scene::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "SceneBase", true, {
            InstanceAccessor("root", &Scene::GetRoot, nullptr),
            InstanceAccessor("stage", &Scene::GetStage, nullptr),
            InstanceAccessor(SymbolFor(env, "graphicsContext"), &Scene::GetGraphicsContext, nullptr),
            InstanceMethod(SymbolFor(env, "attach"), &Scene::Attach),
            InstanceMethod(SymbolFor(env, "detach"), &Scene::Detach),
            InstanceMethod(SymbolFor(env, "destroy"), &Scene::Destroy),
            InstanceMethod(SymbolFor(env, "frame"), &Scene::Frame),
        });
    }

    return constructor.Value();
}

void Scene::Attach(const CallbackInfo& info) {
    // TODO: attach gc?
    // TODO: dirty composite?
    this->width = this->graphicsContext->GetWidth();
    this->height = this->graphicsContext->GetHeight();
    this->viewportMin = std::min(this->width, this->height);
    this->viewportMax = std::max(this->width, this->height);
    this->isViewportSizeDirty = true; // TODO: remove?
    this->isAttached = true;
}

void Scene::Detach(const CallbackInfo& info) {
    // TODO: exceptions?

    this->isAttached = false;
}

void Scene::Frame(const CallbackInfo& info) {
    // TODO: UpdateViewportAndRemUnits();
    if (this->isViewportSizeDirty || this->isRootFontSizeDirty) {
        if (this->isRootFontSizeDirty) {
            this->rootFontSize = ComputeFontSize(this->root->style->fontSize, this, DEFAULT_REM_FONT_SIZE);
        }

        SceneNode::Visit(this->root, [this](SceneNode* node) {
            if (node->style != nullptr) {
                node->style->UpdateDependentProperties(this->isRootFontSizeDirty, this->isViewportSizeDirty);
            }
        });

        this->isViewportSizeDirty = this->isRootFontSizeDirty = false;
    }

    // TODO: RunYogaNodeLayout();
    if (YGNodeIsDirty(this->root->ygNode)) {
        YGNodeCalculateLayout(this->root->ygNode, this->width, this->height, YGDirectionLTR);
        YGTraversePreOrder(this->root->ygNode, [](YGNodeRef node) { node->setHasNewLayout(false); });
    }

    // TODO: RunSceneNodeLayout();


    if (!this->isAttached) {
        return;
    }

    auto renderer{ this->GetRenderer() };

    // TODO: RunSceneNodePaint()
    if (!this->paintRequests.empty()) {
        for (auto& node : this->paintRequests) {
            node->Paint(this->graphicsContext);
        }

        this->paintRequests.clear();
    }

    // TODO: RunComposite()
    if (this->hasCompositeRequest) {
        renderer->Reset();

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

Napi::Value Scene::GetGraphicsContext(const Napi::CallbackInfo &info) {
    return this->graphicsContext->Value();
}

void Scene::Destroy(const Napi::CallbackInfo &info) {
    this->root = SafeObjectWrap<SceneNode>::RemoveRef(this->root, [](SceneNode* node) { node->Destroy(); });
    this->stage = Stage::RemoveRef(this->stage);
    this->graphicsContext = GraphicsContext::RemoveRef(this->graphicsContext);
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

void Scene::OnRootFontSizeChange(float newRootFontSize) noexcept {
    if (!Equals(this->rootFontSize, newRootFontSize)) {
        this->isRootFontSizeDirty = true;
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
