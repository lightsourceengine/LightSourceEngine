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
#include <ls/Math.h>
#include <ls/Style.h>
#include <ls/RootSceneNode.h>
#include <ls/yoga-ext.h>

#include <ls/bindings/JSStage.h>

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
}

void Scene::Attach() noexcept {
    const auto w{ this->graphicsContext->GetWidth() };
    const auto h{ this->graphicsContext->GetHeight() };

    if (w != this->width || h != this->height) {
        this->width = w;
        this->height = h;
        this->isViewportSizeDirty = true;
    }

    // TODO: remove
    this->viewportMin = std::min(this->width, this->height);
    this->viewportMax = std::max(this->width, this->height);

    this->styleResolver = {
        static_cast<float>(this->width),
        static_cast<float>(this->height),
        this->rootFontSize
    };

    this->renderingContext2D.renderer = this->graphicsContext->GetRenderer();
    this->isAttached = true;

    this->RequestComposite();

    // TODO: attach graphics context?
}

void Scene::Detach() noexcept {
    // TODO: detach graphics context?

    this->renderingContext2D.renderer = nullptr;
    this->isAttached = false;
}

void Scene::Frame() {
    if (!this->isAttached) {
        return;
    }

    this->PropagateViewportAndRootFontSizeChanges();
    this->ComputeBoundingBoxLayout();
    this->ExecuteStyleLayoutRequests();
    this->ExecutePaintRequests();
    this->Composite();
}

void Scene::SetRoot(RootSceneNode* rootSceneNode) {
    LS_EXPECT_NULL(this->root, "root has already been set");

    this->root = rootSceneNode;
    this->root->Ref();
}

void Scene::SetStage(const StageRef& stageRef) {
    LS_EXPECT_NULL(this->stage, "stage has already been set");

    this->stage = stageRef;
}

void Scene::SetGraphicsContext(GraphicsContext* graphicsContext) {
    LS_EXPECT_NULL(this->graphicsContext, "graphics context has already been set");

    this->graphicsContext = graphicsContext;
    this->graphicsContext->Ref();
}

void Scene::Destroy() noexcept {
    // TODO: destroy graphics context?
    this->root = SafeObjectWrap<SceneNode>::RemoveRef(this->root, [](SceneNode* node) { node->Destroy(); });
    this->graphicsContext = GraphicsContext::RemoveRef(this->graphicsContext);
    this->stage = nullptr;
}

void Scene::OnRootFontSizeChange() noexcept {
    if (!this->root || !this->root->style) {
        return;
    }

    auto newRootFontSize{ this->styleResolver.Update(this->root->style->fontSize, this->width, this->height) };

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

    this->rootFontSize = this->styleResolver.Update(this->root->style->fontSize, this->width, this->height);

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
        node->Paint(&this->renderingContext2D);
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
    this->compositeContext.renderer = this->GetRenderer();
    this->compositeContext.Reset();
    this->CompositePreorder(this->root, &this->compositeContext);
    renderer->Present();
}

void Scene::CompositePreorder(SceneNode* node, CompositeContext* context) {
    if (node->IsHidden()) {
        return;
    }

    const auto boxStyle{ Style::OrEmpty(node->style) };
    const auto box{ YGNodeGetBox(node->ygNode) };
    const auto clip{ boxStyle->overflow == YGOverflowHidden };

    if (boxStyle->transform.empty()) {
        context->PushMatrix(Matrix::Translate(box.x, box.y));
    } else {
        context->PushMatrix(Matrix::Translate(box.x, box.y)
            * this->GetStyleResolver().ResolveTransform(boxStyle, box));
    }

    context->PushOpacity(boxStyle->opacity.AsFloat(1.f));

    if (clip) {
        context->PushClipRect(box);
    }

    if (!IsEmpty(box)) {
        node->Composite(context);
    }

    if (node->HasChildren()) {
        for (auto child : node->SortChildrenByStackingOrder()) {
            CompositePreorder(child, context);
        }
    }

    if (clip) {
        context->PopClipRect();
    }

    context->PopOpacity();
    context->PopMatrix();
}

} // namespace ls
