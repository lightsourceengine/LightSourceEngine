/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/Rect.h>
#include <lse/SceneNode.h>
#include <lse/Texture.h>

namespace lse {

class BoxSceneNode final : public SceneNode {
 public:
  explicit BoxSceneNode(Scene* scene);
  ~BoxSceneNode() override = default;

  void OnStylePropertyChanged(StyleProperty property) override;
  void OnFlexBoxLayoutChanged() override;
  void OnComputeStyle() override;
  void OnComposite(CompositeContext* composite) override;
  void OnDestroy() override;

 private:
  static void ImageStatusListener(void* owner, Image* image) noexcept;

 private:
  Image* backgroundImage{};
  ImageRect backgroundImageRect{};
};

} // namespace lse
