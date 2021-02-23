/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
