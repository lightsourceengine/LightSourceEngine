/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/SceneNode.h>
#include <lse/TextBlock.h>
#include <lse/types.h>

namespace lse {

class TextSceneNode final : public Napi::SafeObjectWrap<TextSceneNode>, public SceneNode {
 public:
  TextSceneNode(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<TextSceneNode>(info) {}
  ~TextSceneNode() override = default;

  void Constructor(const Napi::CallbackInfo& info) override;

  static Napi::Function GetClass(Napi::Env env);
  Napi::Value GetText(const Napi::CallbackInfo& info);
  void SetText(const Napi::CallbackInfo& info, const Napi::Value& value);

  void OnStylePropertyChanged(StyleProperty property) override;
  void OnBoundingBoxChanged() override;
  void OnStyleLayout() override;
  YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;

  void Paint(RenderingContext2D* context) override;
  void Composite(CompositeContext* composite) override;
  void Destroy() override;

 private:
  bool SetFont(Style* style);
  void ClearFontFaceResource();

 private:
  std::string text{};
  FontFaceRef fontFace{};
  TextBlock block{};
};

} // namespace lse
