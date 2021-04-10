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
