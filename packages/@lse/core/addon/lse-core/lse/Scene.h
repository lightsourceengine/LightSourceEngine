/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <lse/CompositeContext.h>
#include <lse/StyleContext.h>
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
};

} // namespace lse
