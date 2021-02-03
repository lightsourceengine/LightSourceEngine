/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/Scene.h>

#include <lse/Stage.h>
#include <lse/Renderer.h>
#include <lse/GraphicsContext.h>
#include <lse/Log.h>
#include <lse/Math.h>
#include <lse/Style.h>
#include <lse/RootSceneNode.h>
#include <lse/yoga-ext.h>
#include <lse/StyleContext.h>

#include <lse/bindings/JSStage.h>

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

namespace lse {

Scene::~Scene() noexcept {
  if (isAttached) {
    LOG_WARN("scene is still attached");
  }
}

void Scene::Attach() noexcept {
  this->graphicsContext->Attach();

  const auto w{ this->graphicsContext->GetWidth() };
  const auto h{ this->graphicsContext->GetHeight() };

  if (w != this->width || h != this->height) {
    this->width = w;
    this->height = h;
    this->isViewportSizeDirty = true;
  }

  this->SyncStyleContext();

  this->renderingContext2D.renderer = this->graphicsContext->GetRenderer();
  this->isAttached = true;

  this->RequestComposite();
}

void Scene::Detach() noexcept {
  this->graphicsContext->Detach();
  this->renderingContext2D.renderer = nullptr;
  this->isAttached = false;
}

void Scene::Frame() {
  if (!this->isAttached) {
    return;
  }

  this->PropagateViewportAndRootFontSizeChanges();
  this->ExecuteStyleLayoutRequests();
  this->ComputeBoundingBoxLayout();
  this->ExecutePaintRequests();
  this->Composite();
}

void Scene::SetRoot(RootSceneNode* rootSceneNode) {
  LSE_EXPECT_NULL(this->root, "root has already been set");

  this->root = rootSceneNode;
  this->root->Ref();
}

void Scene::SetStage(const StageRef& stageRef) {
  LSE_EXPECT_NULL(this->stage, "stage has already been set");

  this->stage = stageRef;
}

void Scene::SetGraphicsContext(GraphicsContextRef& context) {
  LSE_EXPECT_NULL(this->graphicsContext, "graphics context has already been set");

  this->graphicsContext = context;
}

void Scene::Destroy() noexcept {
  // TODO: destroy graphics context?
  this->isAttached = false;
  this->root = SafeObjectWrap<SceneNode>::RemoveRef(this->root, [](SceneNode* node) { node->Destroy(); });
  this->graphicsContext = nullptr;
  this->stage = nullptr;
}

void Scene::OnRootFontSizeChange() noexcept {
  if (!this->root) {
    return;
  }

  if (this->SyncStyleContext()) {
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

  this->SyncStyleContext();

  SceneNode::Visit(this->root, [this](SceneNode* node) {
    if (node->style != nullptr) {
      node->style->OnMediaChange(this->isRootFontSizeDirty, this->isViewportSizeDirty);
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

  const auto boxStyle{ Style::Or(node->style) };
  const auto box{ YGNodeGetBox(node->ygNode) };
  const auto clip{ boxStyle->GetEnum(StyleProperty::overflow) == YGOverflowHidden };

  if (boxStyle->IsEmpty(StyleProperty::transform)) {
    context->PushMatrix(Matrix::Translate(box.x, box.y));
  } else {
    context->PushMatrix(Matrix::Translate(box.x, box.y)
                            * this->GetStyleContext()->ComputeTransform(boxStyle, box));
  }

  context->PushOpacity(GetStyleContext()->ComputeOpacity(boxStyle));

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

bool Scene::SyncStyleContext() {
  this->styleContext.SetViewportSize(this->width, this->height);

  const auto size = this->styleContext.ComputeFontSize(Style::Or(this->root->style), true);

  if (!Equals(size, this->lastRootFontSize)) {
    this->lastRootFontSize = size;
    this->styleContext.SetRootFontSize(size);

    return true;
  }

  return false;
}

} // namespace lse
