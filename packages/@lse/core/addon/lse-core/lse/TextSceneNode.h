/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/SceneNode.h>
#include <lse/TextBlock.h>

namespace lse {

class TextSceneNode final : public SceneNode {
 public:
  explicit TextSceneNode(Scene* scene);
  ~TextSceneNode() override = default;

  bool IsLeaf() const noexcept override { return true; }

  const std::string& GetText() const;
  void SetText(std::string&& text);

  void OnComposite(CompositeContext* ctx) override;
  void OnStylePropertyChanged(StyleProperty property) override;
  void OnFlexBoxLayoutChanged() override;
  void OnComputeStyle() override;
  void OnDetach() override;

  YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;

  void OnDestroy() override;

 private:
  void DrawText(CompositeContext* ctx);
  bool SetFont(Style* style);
  void ClearFontFaceResource();

 private:
  std::string text{};
  Font* fontFace{};
  TextBlock block{};
};

} // namespace lse
