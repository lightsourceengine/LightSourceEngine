/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/CompositeContext.h>
#include <lse/StyleContext.h>
#include <lse/RenderingContext2D.h>
#include <lse/GraphicsContext.h>
#include <lse/Reference.h>
#include <lse/Stage.h>
#include <lse/FontManager.h>
#include <lse/ImageManager.h>
#include <lse/StyleEnums.h>

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
  Scene(Stage* stage, FontManager* fontManager, ImageManager* imageManager, GraphicsContext* context);
  ~Scene() override;

  void Attach();
  void Detach();
  void Destroy() noexcept;

  void Frame();
  void SetRoot(RootSceneNode* root);

  Stage* GetStage() const noexcept { return this->stage.Get(); }
  FontManager* GetFontManager() const noexcept;
  ImageManager* GetImageManager() const noexcept;
  Renderer* GetRenderer() const noexcept;

  StyleContext* GetStyleContext() const noexcept { return &this->styleContext; }
  int32_t GetWidth() const noexcept { return this->width; }
  int32_t GetHeight() const noexcept { return this->height; }

  void OnRootFontSizeChange() noexcept;
  void RequestComposite();

  void MarkComputeStyleDirty() noexcept { this->isComputeStyleDirty = true; }
  void MarkCompositeDirty() noexcept { this->isCompositeDirty = true; }

 private:
  void DispatchMediaChange();
  void ComputeStyle();
  void ComputeFlexBoxLayout();
  void Paint();
  void Composite();
  void ComputeStylePostOrder(SceneNode* node);
  void CompositePreOrder(SceneNode* node, CompositeContext* context);
  bool SyncStyleContext();

 private:
  ReferenceHolder<Stage> stage{};
  ReferenceHolder<FontManager> fontManager{};
  ReferenceHolder<ImageManager> imageManager{};
  ReferenceHolder<GraphicsContext> graphicsContext{};
  SceneNode* root{};
  mutable StyleContext styleContext{ 0, 0, 0 };
  int32_t width{};
  int32_t height{};
  float lastRootFontSize{ DEFAULT_REM_FONT_SIZE };
  bool isViewportSizeDirty{ true };
  bool isRootFontSizeDirty{ false };
  bool isComputeStyleDirty{ false };
  bool isCompositeDirty{ false };
  bool isAttached{ false };
  std::vector<SceneNode*> paintRequests;
  CompositeContext compositeContext;
  RenderingContext2D renderingContext2D{};
  std::vector<SceneNode*> children;
};

} // namespace lse
