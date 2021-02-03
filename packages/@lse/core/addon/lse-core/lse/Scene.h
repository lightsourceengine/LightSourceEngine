/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/types.h>
#include <lse/CompositeContext.h>
#include <lse/StyleContext.h>
#include <lse/RenderingContext2D.h>
#include <lse/GraphicsContext.h>

#include <algorithm>
#include <memory>
#include <unordered_set>

namespace lse {

class Renderer;
class RootSceneNode;
class SceneNode;

/**
 * Manages the SceneNode graph and renders frames to the screen.
 */
class Scene {
 public:
  ~Scene() noexcept;

  void Attach() noexcept;
  void Detach() noexcept;
  void Destroy() noexcept;

  void Frame();

  void SetRoot(RootSceneNode* root);
  void SetStage(const StageRef& stage);
  void SetGraphicsContext(GraphicsContextRef& context);
  Stage* GetStage() const noexcept { return this->stage.get(); }
  int32_t GetWidth() const noexcept { return this->width; }
  int32_t GetHeight() const noexcept { return this->height; }
  StyleContext* GetStyleContext() const noexcept { return &this->styleContext; }
  Renderer* GetRenderer() const noexcept;

  void OnRootFontSizeChange() noexcept;
  void RequestPaint(SceneNode* node);
  void RequestStyleLayout(SceneNode* node);
  void RequestComposite();
  void Remove(SceneNode* node);

 private:
  void PropagateViewportAndRootFontSizeChanges();
  void ComputeBoundingBoxLayout();
  void ExecuteStyleLayoutRequests();
  void ExecutePaintRequests();
  void Composite();
  void CompositePreorder(SceneNode* node, CompositeContext* context);
  bool SyncStyleContext();

 private:
  SceneNode* root{};
  StageRef stage{};
  GraphicsContextRef graphicsContext{};
  mutable StyleContext styleContext{ 0, 0, 0 };
  int32_t width{};
  int32_t height{};
  float lastRootFontSize{ DEFAULT_REM_FONT_SIZE };
  bool isViewportSizeDirty{ true };
  bool isRootFontSizeDirty{ false };
  bool isAttached{ false };
  bool hasCompositeRequest{ false };
  std::unordered_set<SceneNode*> paintRequests;
  std::unordered_set<SceneNode*> styleLayoutRequests;
  CompositeContext compositeContext;
  RenderingContext2D renderingContext2D{};
};

} // namespace lse
