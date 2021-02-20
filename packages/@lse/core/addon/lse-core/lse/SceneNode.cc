/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/SceneNode.h>

#include <cassert>
#include <algorithm>
#include <lse/Style.h>
#include <lse/StyleContext.h>
#include <lse/Scene.h>
#include <lse/Stage.h>
#include <lse/CompositeContext.h>
#include <lse/Renderer.h>
#include <lse/RootSceneNode.h>
#include <lse/yoga-ext.h>
#include <lse/Habitat.h>

using Napi::Array;
using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::Value;

namespace lse {

int32_t SceneNode::instanceCount{0};

SceneNode::SceneNode(napi_env env, Scene* scene) : env(env), scene(scene) {
  assert(scene != nullptr);

  this->ygNode = YGNodeNew();
  YGNodeSetContext(this->ygNode, this);
  instanceCount++;
}

int32_t SceneNode::GetInstanceCount() noexcept {
  return SceneNode::instanceCount;
}

float SceneNode::GetX() const noexcept {
  return YGNodeLayoutGetLeft(this->ygNode);
}

float SceneNode::GetY() const noexcept {
  return YGNodeLayoutGetTop(this->ygNode);
}

float SceneNode::GetWidth() const noexcept {
  return YGNodeLayoutGetWidth(this->ygNode);
}

float SceneNode::GetHeight() const noexcept {
  return YGNodeLayoutGetHeight(this->ygNode);
}

void SceneNode::BindStyle(Style* style) noexcept {
  assert(style != nullptr);

  this->style = style;
  // TODO: remove std::function allocation?
  this->style->SetChangeListener([this](StyleProperty property) {
    if (IsYogaProperty(property)) {
      this->GetStyleContext()->SetYogaPropertyValue(this->style, property, this->ygNode);
    } else {
      this->OnStylePropertyChanged(property);
    }
  });
}

void SceneNode::SetHidden(bool value) noexcept {
  this->flags.set(FlagHidden, value);
}

Resources* SceneNode::GetResources() const noexcept {
  assert(this->scene);
  return this->scene->GetStage()->GetResources();
}

SceneNode* SceneNode::GetParent() const noexcept {
  auto parent{ this->ygNode ? this->ygNode->getParent() : nullptr };

  return parent ? YGNodeGetContextAs<SceneNode>(parent) : nullptr;
}

void SceneNode::AppendChild(SceneNode* node) {
  if (this->IsLeaf()) {
    throw std::runtime_error("leaf nodes cannot have children");
  }

  if (node == nullptr || node == this) {
    throw std::runtime_error("invalid node argument");
  }

  if (node->scene != this->scene) {
    throw std::runtime_error("node was created by another scene");
  }

  if (node->GetParent() != nullptr) {
    throw std::runtime_error("node already has a parent");
  }

  YGNodeInsertChild(this->ygNode, node->ygNode, YGNodeGetChildCount(this->ygNode));

  // Add reference for the added child.
  node->Ref();
  // Add reference for the parent.
  this->Ref();
}

void SceneNode::InsertBefore(SceneNode* node, SceneNode* before) {
  if (this->IsLeaf()) {
    throw std::runtime_error("leaf nodes cannot have children");
  }

  if (node == nullptr || node == this) {
    throw std::runtime_error("invalid node argument");
  }

  if (node->scene != this->scene) {
    throw std::runtime_error("node was created by another scene");
  }

  if (node->GetParent() != nullptr) {
    throw std::runtime_error("child already has a parent");
  }

  if (before == nullptr) {
    throw std::runtime_error("before must be a SceneNode");
  }

  auto beforeIndex{ this->GetChildIndex(before) };

  if (beforeIndex < 0) {
    throw std::runtime_error("before argument is not a child");
  }

  YGNodeInsertChild(this->ygNode, node->ygNode, beforeIndex);

  // Add reference for the added child.
  node->Ref();
  // Add reference for the parent.
  this->Ref();
}

void SceneNode::RemoveChild(SceneNode* node) noexcept {
  if (!node || GetChildIndex(node) < 0) {
    return;
  }

  YGNodeRemoveChild(this->ygNode, node->ygNode);

  // Remove reference for the child.
  node->Unref();

  // Remove reference for the parent
  this->Unref();
}

void SceneNode::Paint(CompositeContext* ctx) {
  auto box{YGNodeGetBox(this->ygNode, 0, 0)};

  if (this->target) {
    if (this->target.Width() < box.width || this->target.Height() < box.height) {
      this->target.Destroy();
    }
  }

  if (!this->target) {
    this->target = ctx->renderer->CreateTexture(box.width, box.height, Texture::RenderTarget);

    if (!this->target) {
      return;
    }
  }

  ctx->renderer->SetRenderTarget(this->target);

  this->OnComposite(ctx);
}

void SceneNode::Composite(CompositeContext* ctx) {
  if (this->target) {
    auto box{YGNodeGetBox(this->ygNode)};

    box.width *= ctx->CurrentMatrix().GetScaleX();
    box.height *= ctx->CurrentMatrix().GetScaleY();

    ctx->renderer->DrawImage(
        ctx->CurrentRenderTransform(),
        { 0, 0 },
        box,
        { 0, 0, static_cast<int32_t>(box.width), static_cast<int32_t>(box.height)},
        this->target,
        {});
  } else {
    this->OnComposite(ctx);
  }
}

void SceneNode::Destroy() {
  if (this->ygNode == nullptr) {
    return;
  }

  this->OnDestroy();

  const auto& children{ YGNodeGetChildren(this->ygNode) };

  if (!children.empty()) {
    throw std::runtime_error("Cannot destroy node with children");
  }

  instanceCount--;

  auto parent{ this->GetParent() };

  if (parent) {
    parent->RemoveChild(this);
  }

  if (this->style) {
    this->style->ClearChangeListener();
    this->style->SetParent(nullptr);
    this->style = nullptr;
  }

  this->scene = nullptr;

  YGNodeFree(this->ygNode);
  this->ygNode = nullptr;
}

void SceneNode::OnStylePropertyChanged(StyleProperty property) {
  switch (property) {
    case StyleProperty::transformOriginX:
    case StyleProperty::transformOriginY:
    case StyleProperty::opacity:
    case StyleProperty::transform:
    case StyleProperty::overflow:
      // TODO: if in software mode, compute
      this->MarkCompositeDirty();
      break;
    case StyleProperty::zIndex:
      // TODO: addChild(), removeChild()
      if (this->GetParent()) {
        this->GetParent()->MarkCompositeDirty();
      }
      break;
    default:
      break;
  }
}

int32_t SceneNode::GetChildIndex(SceneNode* node) const noexcept {
  int32_t index{ 0 };

  for (const auto& child : YGNodeGetChildren(this->ygNode)) {
    if (YGNodeGetContextAs<SceneNode>(child) == node) {
      return index;
    }
    index++;
  }

  return -1;
}

int32_t SceneNode::GetZIndex(SceneNode* node) const noexcept {
  return node->style && node->style->GetInteger(StyleProperty::zIndex).value_or(0);
}

const std::vector<SceneNode*>& SceneNode::GetChildrenOrderedByZIndex(std::vector<SceneNode*>& temp) {
  SceneNode* node;
  int32_t lastZ = INT32_MIN;
  int32_t currentZ;
  bool outOfOrder{};

  temp.clear();

  for (const auto& child : YGNodeGetChildren(this->ygNode)) {
    node = YGNodeGetContextAs<SceneNode>(child);

    if (!outOfOrder) {
      currentZ = this->GetZIndex(node);

      if (currentZ < lastZ) {
        outOfOrder = true;
      }

      lastZ = currentZ;
    }

    temp.push_back(node);
  }

  if (outOfOrder) {
    std::stable_sort(
        temp.begin(),
        temp.end(),
        [](SceneNode* a, SceneNode* b) {
          return a->GetZIndex(a) < b->GetZIndex(b);
        });
  }

  return temp;
}

YGSize SceneNode::OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
  return { 0.f, 0.f };
}

bool SceneNode::IsHidden() const noexcept {
  return this->flags.test(FlagHidden);
}

bool SceneNode::IsLayoutOnly() const noexcept {
  return this->flags.test(FlagLayoutOnly);
}

bool SceneNode::IsComputeStyleDirty() const noexcept {
  return this->flags.test(FlagComputeStyleDirty);
}

bool SceneNode::IsCompositeDirty() const noexcept {
  return this->flags.test(FlagCompositeDirty);
}

void SceneNode::MarkComputeStyleDirty() noexcept {
  this->flags.set(FlagComputeStyleDirty);
  this->scene->MarkComputeStyleDirty();
}

void SceneNode::MarkCompositeDirty() noexcept {
  this->flags.set(FlagCompositeDirty);
  this->scene->MarkCompositeDirty();
}

bool SceneNode::HasChildren() const noexcept {
  return !YGNodeGetChildren(this->ygNode).empty();
}

void SceneNode::SetFlag(Flag flag, bool value) noexcept {
  this->flags.set(flag, value);
}

StyleContext* SceneNode::GetStyleContext() const noexcept {
  assert(this->scene);
  return this->scene->GetStyleContext();
}

uint32_t SceneNode::GetChildCount() const noexcept {
  return YGNodeGetChildCount(this->ygNode);
}

SceneNode* SceneNode::GetChildAt(uint32_t index) const noexcept {
  auto child{YGNodeGetChild(this->ygNode, index)};

  if (child) {
    return YGNodeGetContextAs<SceneNode>(child);
  }

  return {};
}

void SceneNode::YogaNodeLayoutEvent(
    const YGNode& node, facebook::yoga::Event::Type event, const facebook::yoga::Event::Data& data) {
  assert(event == Event::NodeLayout);

  if (node.getHasNewLayout()) {
    assert(node.getContext() != nullptr);
    YGNodeGetContextAs<SceneNode>(&node)->OnFlexBoxLayoutChanged();
  }
}

YGSize SceneNode::YogaMeasureCallback(
    YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
  assert(nodeRef->getContext() != nullptr);
  return YGNodeGetContextAs<SceneNode>(nodeRef)->OnMeasure(width, widthMode, height, heightMode);
}

Rect SceneNode::GetBackgroundClipBox(StyleBackgroundClip value) const noexcept {
  switch (value) {
    case StyleBackgroundClipBorderBox:
      return YGNodeGetBox(this->ygNode, 0, 0);
    case StyleBackgroundClipPaddingBox:
      return YGNodeGetBorderBox(this->ygNode);
    case StyleBackgroundClipContentBox:
      return YGNodeGetPaddingBox(this->ygNode);
  }

  return {};
}

void SceneNode::DrawBackground(CompositeContext* ctx, StyleBackgroundClip backgroundClip) const noexcept {
  if (this->style && !this->style->IsEmpty(StyleProperty::backgroundColor)) {
    ctx->renderer->FillRect(
        GetBackgroundClipBox(backgroundClip),
        RenderFilter::OfTint(*this->style->GetColor(StyleProperty::backgroundColor)));
  }
}

void SceneNode::DrawBorder(CompositeContext* ctx) const noexcept {
  if (this->style && !this->style->IsEmpty(StyleProperty::borderColor)) {
    ctx->renderer->StrokeRect(
        YGNodeGetBox(this->ygNode),
        YGNodeGetBorderEdges(this->ygNode),
        RenderFilter::OfTint(*this->style->GetColor(StyleProperty::borderColor)));
  }
}

} // namespace lse
