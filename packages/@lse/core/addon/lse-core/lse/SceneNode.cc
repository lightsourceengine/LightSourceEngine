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

int32_t SceneNode::instanceCount{ 0 };

SceneNode::SceneNode(napi_env env, Scene* scene) {
  assert(scene != nullptr);

  this->env = env;
  this->scene = scene;
  this->flags.set(FlagLayoutOnly, true);
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
  this->sortedChildren.clear();

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
  this->sortedChildren.clear();

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
  this->sortedChildren.clear();

  // Remove reference for the child.
  node->Unref();

  // Remove reference for the parent
  this->Unref();
}

void SceneNode::Destroy() {
  if (this->ygNode == nullptr) {
    return;
  }

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

  this->scene->Remove(this);
  this->scene = nullptr;

  this->sortedChildren.clear();
  YGNodeFree(this->ygNode);
  this->ygNode = nullptr;
}

void SceneNode::OnStylePropertyChanged(StyleProperty property) {
  switch (property) {
    case StyleProperty::transformOriginX:
    case StyleProperty::transformOriginY:
    case StyleProperty::opacity:
      this->RequestComposite();
      break;
    case StyleProperty::transform:
    case StyleProperty::zIndex:
      if (this->GetParent()) {
        this->GetParent()->sortedChildren.clear();
      }
      this->RequestComposite();
      break;
    default:
      break;
  }
}

YGSize SceneNode::OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
  return {};
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

void SceneNode::RequestPaint() {
  this->scene->RequestPaint(this);
}

void SceneNode::RequestStyleLayout() {
  this->scene->RequestStyleLayout(this);
}

void SceneNode::RequestComposite() {
  this->scene->RequestComposite();
}

const std::vector<SceneNode*>& SceneNode::SortChildrenByStackingOrder() {
  const auto compare = [](SceneNode* a, SceneNode* b) {
    auto aStyle = Style::Or(a->style);
    auto bStyle = Style::Or(b->style);
    const auto aHasTransform{ aStyle->Exists(StyleProperty::transform) };

    if (aHasTransform != bStyle->Exists(StyleProperty::transform)) {
      // nodes with transform always drawn above nodes with no transform
      //
      // if a has transform and b has no transform, then a is less than b
      // if b has transform and a has no transform, then a is less than b
      return !aHasTransform;
    }

    // by default, nodes have a z-index of 0
    return aStyle->GetInteger(StyleProperty::zIndex).value_or(0) <
        bStyle->GetInteger(StyleProperty::zIndex).value_or(0);
  };

  if (!this->sortedChildren.empty()) {
    // already sorted
    return this->sortedChildren;
  }

  this->sortedChildren.reserve(YGNodeGetChildCount(this->ygNode));

  for (const auto& child : YGNodeGetChildren(this->ygNode)) {
    this->sortedChildren.push_back(YGNodeGetContextAs<SceneNode>(child));
  }

  // use stable_sort to preserve the original order of nodes when z-indexes are the same
  std::stable_sort(this->sortedChildren.begin(), this->sortedChildren.end(), compare);

  return this->sortedChildren;
}

bool SceneNode::IsLeaf() const noexcept {
  return this->flags.test(FlagLeaf);
}

bool SceneNode::IsHidden() const noexcept {
  return this->flags.test(FlagHidden);
}

bool SceneNode::IsLayoutOnly() const noexcept {
  return this->flags.test(FlagLayoutOnly);
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

YGSize SceneNode::YogaMeasureCallback(
    YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
  auto sceneNode{ static_cast<SceneNode*>(nodeRef->getContext()) };

  assert(sceneNode != nullptr);

  return sceneNode->OnMeasure(width, widthMode, height, heightMode);
}

void SceneNode::YogaNodeLayoutEvent(
    const YGNode& node, facebook::yoga::Event::Type event, const facebook::yoga::Event::Data& data) {
  assert(event == Event::NodeLayout);
  assert(node.getContext() != nullptr);

  if (node.getHasNewLayout()) {
    static_cast<lse::SceneNode*>(node.getContext())->OnBoundingBoxChanged();
  }
}

} // namespace lse
