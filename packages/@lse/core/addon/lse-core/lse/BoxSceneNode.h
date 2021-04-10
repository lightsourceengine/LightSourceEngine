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

#include <lse/Rect.h>
#include <lse/SceneNode.h>
#include <lse/Texture.h>
#include <lse/StyleEnums.h>

namespace lse {

class BoxSceneNode final : public SceneNode {
 public:
  explicit BoxSceneNode(Scene* scene);
  ~BoxSceneNode() override = default;

  void OnStylePropertyChanged(StyleProperty property) override;
  void OnFlexBoxLayoutChanged() override;
  void OnComputeStyle() override;
  void OnComposite(CompositeContext* ctx) override;
  void OnDestroy() override;

 private:
  static void ImageStatusListener(void* owner, Image* image) noexcept;
  void DrawBackgroundImageRepeat(CompositeContext* ctx, StyleBackgroundRepeat repeat);

 private:
  Image* backgroundImage{};
  ImageRect backgroundImageRect{};
};

} // namespace lse
