/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/Rect.h>
#include <lse/types.h>
#include <lse/SceneNode.h>

namespace lse {

class Resource;
class ImageStatusCallback;

class ImageSceneNode final : public SceneNode {
 public:
  explicit ImageSceneNode(napi_env env, Scene* scene);
  ~ImageSceneNode() override = default;

  bool IsLeaf() const noexcept override { return true; }

  const std::string& GetSource() const noexcept;
  void SetSource(napi_env env, std::string&& value) noexcept;

  bool HasImageStatusCallback() const noexcept;
  void SetImageStatusCallback(std::unique_ptr<ImageStatusCallback>&& callback) noexcept;

  void OnDetach() override;
  void OnComputeStyle() override;
  void OnComposite(CompositeContext* composite) override;
  void OnStylePropertyChanged(StyleProperty property) override;
  void OnFlexBoxLayoutChanged() override;

  YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;

  void OnDestroy() override;

 private:
  void ClearResource();

 private:
  std::string src{};
  ImageRef image{};
  ImageRect imageRect{};
  std::unique_ptr<ImageStatusCallback> imageStatusCallback{};
};

class ImageStatusCallback {
 public:
  virtual ~ImageStatusCallback() = default;
  virtual void Invoke(Resource* image, const std::string& errorMessage) = 0;
};

} // namespace lse
