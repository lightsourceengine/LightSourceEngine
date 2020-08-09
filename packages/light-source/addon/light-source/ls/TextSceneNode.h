/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/SceneNode.h>
#include <ls/TextLayout.h>
#include <ls/Resources.h>

namespace ls {

class TextSceneNode : public Napi::SafeObjectWrap<TextSceneNode>, public SceneNode {
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

    void Paint(GraphicsContext* graphicsContext) override;
    void Composite(CompositeContext* composite) override;

 private:
    bool SetFont(Style* style);
    void ClearFontFaceResource();
    void DestroyRecursive() override;
    YGSize Measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);
    void QueueTextLayout() noexcept;

 private:
    std::string text;
    FontFace* fontFace{};
    TextLayout layout;
};

} // namespace ls
