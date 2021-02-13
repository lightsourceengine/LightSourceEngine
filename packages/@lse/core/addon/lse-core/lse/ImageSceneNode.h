/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/Rect.h>
#include <lse/ResourceProgress.h>
#include <lse/types.h>
#include <lse/SceneNode.h>

namespace lse {

class ImageSceneNode final : public SceneNode {
 public:
  explicit ImageSceneNode(napi_env env, Scene* scene);
  ~ImageSceneNode() override = default;

  const std::string& GetSource() const noexcept;
  void SetSource(napi_env env, std::string&& value) noexcept;

  napi_value GetOnLoadCallback(napi_env env) noexcept;
  void SetOnLoadCallback(napi_env env, napi_value value) noexcept;

  napi_value GetOnErrorCallback(napi_env env) noexcept;
  void SetOnErrorCallback(napi_env env, napi_value value) noexcept;

  void OnStylePropertyChanged(StyleProperty property) override;
  void OnBoundingBoxChanged() override;
  void OnStyleLayout() override;
  YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;

  void Paint(RenderingContext2D* context) override;
  void Composite(CompositeContext* composite) override;
  void Destroy() override;

 private:
  void ClearResource();

 private:
  std::string src;
  ImageRef image{};
  ImageRect imageRect{};
  ResourceProgress resourceProgress;
};

} // namespace lse
