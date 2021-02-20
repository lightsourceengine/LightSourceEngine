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
#include <lse/Timer.h>

namespace lse {

Scene::Scene(Stage* stage, FontManager* fontManager, GraphicsContext* context)
: stage(stage), fontManager(fontManager), graphicsContext(context) {
  this->children.reserve(32);
  this->paintRequests.reserve(32);
}

Scene::~Scene() {
  if (this->isAttached) {
    LOG_WARN("scene is still attached");
  }
}

void Scene::SetRoot(RootSceneNode* root) {
  assert(this->root == nullptr);

  root->Ref();
  this->root = root;
}

void Scene::Attach() {
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

void Scene::Detach() {
  if (this->root) {
    SceneNode::Visit(this->root, [](SceneNode* node){ node->OnDetach(); });
  }

  this->graphicsContext->Detach();
  this->renderingContext2D.renderer = nullptr;
  this->isAttached = false;
}

void Scene::Frame() {
  if (!this->isAttached) {
    return;
  }

  // media changes, root font size and/or viewport change
  // DispatchMediaChangeEvent
  this->DispatchMediaChange();

  // flex box
  this->ComputeFlexBoxLayout();

  // compute
  this->ComputeStyle();

  this->Paint();

  // composite
  this->Composite();
}

void Scene::Destroy() noexcept {
  this->isAttached = false;
  if (this->root) {
    this->root->Unref();
    this->root = nullptr;
  }
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

Renderer* Scene::GetRenderer() const noexcept {
  return this->graphicsContext->GetRenderer();
}

FontManager* Scene::GetFontManager() const noexcept {
  return this->fontManager;
}

void Scene::RequestComposite() {
  this->isCompositeDirty = true;
}

void Scene::DispatchMediaChange() {
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

void Scene::ComputeStyle() {
  if (this->isComputeStyleDirty) {
    ComputeStylePostOrder(this->root);
  }
}

void Scene::ComputeStylePostOrder(SceneNode* node) {
  if (!node->IsLeaf()) {
    auto count{ node->GetChildCount() };

    for (uint32_t i = 0; i < count; i++) {
      ComputeStylePostOrder(node->GetChildAt(i));
    }
  }

  if (node->IsComputeStyleDirty()) {
    node->OnComputeStyle();
    node->flags.set(SceneNode::FlagComputeStyleDirty, false);
  }
}

void Scene::ComputeFlexBoxLayout() {
  if (YGNodeIsDirty(this->root->ygNode)) {
    YGNodeCalculateLayout(this->root->ygNode, this->width, this->height, YGDirectionLTR);
    YGTraversePreOrder(this->root->ygNode, [](YGNodeRef node) { node->setHasNewLayout(false); });
  }
}

void Scene::Paint() {
  for (auto node : this->paintRequests) {
    this->compositeContext.Reset(this->GetRenderer());
    node->Paint(&this->compositeContext);
  }
}

void Scene::Composite() {
  if (!this->isCompositeDirty) {
    return;
  }

  this->isCompositeDirty = false;

  auto renderer{ this->GetRenderer() };

  renderer->Reset();
  this->compositeContext.Reset(renderer);
  this->CompositePreOrder(this->root, &this->compositeContext);
  renderer->Present();
}

void Scene::CompositePreOrder(SceneNode* node, CompositeContext* context) {
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
    node->flags.set(SceneNode::FlagCompositeDirty, false);
  }

  if (node->HasChildren()) {
    for (auto child : node->GetChildrenOrderedByZIndex(this->children)) {
      CompositePreOrder(child, context);
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
