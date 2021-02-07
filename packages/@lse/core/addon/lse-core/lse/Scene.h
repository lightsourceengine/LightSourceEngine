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
#include <lse/Reference.h>
#include <lse/Stage.h>

#include <algorithm>
#include <memory>
#include <phmap.h>

namespace lse {

class Renderer;
class RootSceneNode;
class SceneNode;

/**
 * Manages the SceneNode graph and renders frames to the screen.
 */
class Scene : public Reference {
 public:
  Scene(Stage* stage, GraphicsContext* context);
  ~Scene() override;

  void Attach();
  void Detach();
  void Destroy() noexcept;

  void Frame();

  void SetRoot(RootSceneNode* root);
  Stage* GetStage() const noexcept { return this->stage.Get(); }
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
  ReferenceHolder<Stage> stage{};
  ReferenceHolder<GraphicsContext> graphicsContext{};
  SceneNode* root{};
  mutable StyleContext styleContext{ 0, 0, 0 };
  int32_t width{};
  int32_t height{};
  float lastRootFontSize{ DEFAULT_REM_FONT_SIZE };
  bool isViewportSizeDirty{ true };
  bool isRootFontSizeDirty{ false };
  bool isAttached{ false };
  bool hasCompositeRequest{ false };
  phmap::flat_hash_set<SceneNode*> paintRequests;
  phmap::flat_hash_set<SceneNode*> styleLayoutRequests;
  CompositeContext compositeContext;
  RenderingContext2D renderingContext2D{};
};

} // namespace lse
