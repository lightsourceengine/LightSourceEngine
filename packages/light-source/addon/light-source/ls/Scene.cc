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

Scene::~Scene() {
    if (isAttached) {
        LOG_WARN("scene is still attached");
    }

    this->RemoveInternalReferences();
}

void Scene::Constructor(const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);
    auto rootSceneNode{ RootSceneNode::GetClass(env).New({ info.This() }) };

    this->stage = Stage::CastRef(info[0]);

    if (!this->stage) {
        RemoveInternalReferences();
        throw Error::New(env, "Invalid stage argument.");
    }

    this->graphicsContext = GraphicsContext::CastRef(info[1]);

    if (!this->graphicsContext) {
        RemoveInternalReferences();
        throw Error::New(env, "Invalid graphics context argument.");
    }

    this->root = RootSceneNode::CastRef(rootSceneNode);

    if (!this->root) {
        RemoveInternalReferences();
        throw Error::New(env, "Error creating root scene node.");
    }
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
    const auto w{ this->graphicsContext->GetWidth() };
    const auto h{ this->graphicsContext->GetHeight() };

    if (w != this->width || h != this->height) {
        this->width = w;
        this->height = h;
        this->isViewportSizeDirty = true;
    }

    this->viewportMin = std::min(this->width, this->height);
    this->viewportMax = std::max(this->width, this->height);
    this->isAttached = true;

    this->RequestComposite();

    // TODO: attach graphics context?
}

void Scene::Detach(const CallbackInfo& info) {
    // TODO: detach graphics context?

    this->isAttached = false;
}

void Scene::Frame(const CallbackInfo& info) {
    if (!this->isAttached) {
        return;
    }

    this->PropagateViewportAndRootFontSizeChanges();
    this->ComputeBoundingBoxLayout();
    this->ExecuteStyleLayoutRequests();
    this->ExecutePaintRequests();
    this->Composite();
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
    this->RemoveInternalReferences();
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

void Scene::RequestPaint(SceneNode* node) {
    this->paintRequests.insert(node);
}

void Scene::RequestStyleLayout(SceneNode* node) {
    this->styleLayoutRequests.insert(node);
}

Renderer* Scene::GetRenderer() const noexcept {
    return this->graphicsContext->GetRenderer();
}

void Scene::RequestComposite() {
    this->hasCompositeRequest = true;
}

void Scene::Remove(SceneNode* node) {
    this->paintRequests.erase(node);
    this->styleLayoutRequests.erase(node);
}

void Scene::PropagateViewportAndRootFontSizeChanges() {
    if (!this->isViewportSizeDirty && !this->isRootFontSizeDirty) {
        return;
    }

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

void Scene::ComputeBoundingBoxLayout() {
    if (YGNodeIsDirty(this->root->ygNode)) {
        YGNodeCalculateLayout(this->root->ygNode, this->width, this->height, YGDirectionLTR);
        YGTraversePreOrder(this->root->ygNode, [](YGNodeRef node) { node->setHasNewLayout(false); });
    }
}

void Scene::ExecuteStyleLayoutRequests() {
    if (this->styleLayoutRequests.empty()) {
        return;
    }

    for (auto& node : this->styleLayoutRequests) {
        node->OnStyleLayout();
    }

    this->styleLayoutRequests.clear();
}

void Scene::ExecutePaintRequests() {
    if (this->paintRequests.empty()) {
        return;
    }

    for (auto& node : this->paintRequests) {
        node->Paint(this->graphicsContext);
    }

    this->paintRequests.clear();
}

void Scene::Composite() {
    if (!this->hasCompositeRequest) {
        return;
    }

    this->hasCompositeRequest = false;

    auto renderer{ this->GetRenderer() };

    renderer->Reset();
    this->compositeContext.Reset(renderer);
    this->root->Composite(&this->compositeContext);
    renderer->Present();
}

void Scene::RemoveInternalReferences() noexcept {
    // TODO: destroy graphics context?
    this->graphicsContext = GraphicsContext::RemoveRef(this->graphicsContext);
    this->root = SafeObjectWrap<SceneNode>::RemoveRef(this->root, [](SceneNode* node) { node->Destroy(); });
    this->stage = Stage::RemoveRef(this->stage);
}

} // namespace ls
