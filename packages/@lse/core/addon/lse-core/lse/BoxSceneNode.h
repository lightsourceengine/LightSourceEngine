/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/Rect.h>
#include <lse/types.h>
#include <lse/SceneNode.h>
#include <lse/Texture.h>

namespace lse {

class BoxSceneNode final : public SceneNode {
 public:
  explicit BoxSceneNode(napi_env env, Scene* scene);
  ~BoxSceneNode() override = default;

  void OnStylePropertyChanged(StyleProperty property) override;
  void OnStyleReset() override;
  void OnBoundingBoxChanged() override;
  void OnStyleLayout() override;
  void Paint(RenderingContext2D* context) override;
  void Composite(CompositeContext* composite) override;
  void Destroy() override;

 private:
  void PaintBorderRadius(RenderingContext2D* context);
  void PaintBackgroundRepeat(RenderingContext2D* context);
  void UpdateBackgroundImage(const std::string& backgroundUri);
  void ClearBackgroundImageResource();
  Rect GetBackgroundClipBox(StyleBackgroundClip value) const noexcept;

 private:
  ImageRef backgroundImage{};
  ImageRect backgroundImageRect{};
  Texture paintTarget{};
};

} // namespace lse
