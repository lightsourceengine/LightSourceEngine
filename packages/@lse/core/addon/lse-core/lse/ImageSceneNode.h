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

namespace lse {

class Image;
class ImageStatusCallback;
struct ImageRequest;

class ImageSceneNode final : public SceneNode {
 public:
  explicit ImageSceneNode(Scene* scene);
  ~ImageSceneNode() override = default;

  bool IsLeaf() const noexcept override { return true; }

  void SetSource(const ImageRequest& request) noexcept;
  void ResetSource() noexcept;

  bool HasImageStatusCallback() const noexcept;
  void SetImageStatusCallback(std::unique_ptr<ImageStatusCallback>&& callback) noexcept;

  void OnComputeStyle() override;
  void OnComposite(CompositeContext* composite) override;
  void OnStylePropertyChanged(StyleProperty property) override;
  void OnFlexBoxLayoutChanged() override;

  YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;

  void OnDestroy() override;

 private:
  static void ImageStatusListener(void* owner, Image* image) noexcept;

 private:
  std::string src{};
  Image* image{};
  ImageRect imageRect{};
  std::unique_ptr<ImageStatusCallback> imageStatusCallback{};
};

class ImageStatusCallback {
 public:
  virtual ~ImageStatusCallback() = default;
  virtual void Invoke(Image* image) = 0;
};

} // namespace lse
