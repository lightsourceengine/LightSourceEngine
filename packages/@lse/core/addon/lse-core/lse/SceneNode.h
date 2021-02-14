/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/types.h>
#include <lse/Reference.h>
#include <lse/Scene.h>
#include <lse/Style.h>
#include <lse/yoga-ext.h>
#include <event/event.h>
#include <napi-ext.h>
#include <lse/StyleEnums.h>
#include <lse/Resources.h>
#include <bitset>

namespace lse {

class CompositeContext;
class GraphicsContext;
class Renderer;
class RenderingContext2D;
class SceneNode;
class Stage;
class StyleContext;
class Texture;

class SceneNode : public Reference {
 public:
  // TODO: remove env when image loading no longer needs it
  explicit SceneNode(napi_env env, Scene* scene);
  ~SceneNode() override = default;

  float GetX() const noexcept;
  float GetY() const noexcept;
  float GetWidth() const noexcept;
  float GetHeight() const noexcept;
  void BindStyle(Style* style) noexcept;
  void SetHidden(bool value) noexcept;

  void AppendChild(SceneNode* node);
  void InsertBefore(SceneNode* node, SceneNode* before);
  void RemoveChild(SceneNode* node) noexcept;

  virtual void OnStylePropertyChanged(StyleProperty property);
  virtual void OnStyleReset() {}
  virtual void OnBoundingBoxChanged() {}
  virtual void OnStyleLayout() {}
  virtual void OnDetach() = 0;

  virtual YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);
  virtual void Paint(RenderingContext2D* context) = 0;
  virtual void Composite(CompositeContext* composite) = 0;
  virtual void Destroy();

  Resources* GetResources() const noexcept;
  SceneNode* GetParent() const noexcept;
  bool IsLeaf() const noexcept;
  bool IsHidden() const noexcept;
  bool IsLayoutOnly() const noexcept;

  bool HasChildren() const noexcept;

  template<typename Callable>
  static void Visit(SceneNode* node, const Callable& func);

  static YGSize YogaMeasureCallback(
      YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);

  static void YogaNodeLayoutEvent(
      const YGNode& node, facebook::yoga::Event::Type event, const facebook::yoga::Event::Data& data);

  static int32_t GetInstanceCount() noexcept;

 protected:
  enum Flag : uint32_t {
    FlagHidden,
    FlagLayoutOnly,
    FlagLeaf
  };

  void RequestPaint();
  void RequestStyleLayout();
  void RequestComposite();
  const std::vector<SceneNode*>& SortChildrenByStackingOrder();
  void SetFlag(Flag flag, bool value) noexcept;
  StyleContext* GetStyleContext() const noexcept;

 private:
  int32_t GetChildIndex(SceneNode* node) const noexcept;

 protected:
  static int32_t instanceCount;
  napi_env env{};
  YGNodeRef ygNode{};
  ReferenceHolder<Scene> scene{};
  ReferenceHolder<Style> style{};
  std::vector<SceneNode*> sortedChildren;
  std::bitset<8> flags;

  friend Scene;
};

template<typename Callable>
void SceneNode::Visit(SceneNode* node, const Callable& func) {
  func(node);

  for (const auto& child : YGNodeGetChildren(node->ygNode)) {
    Visit(YGNodeGetContextAs<SceneNode>(child), func);
  }
}

} // namespace lse
