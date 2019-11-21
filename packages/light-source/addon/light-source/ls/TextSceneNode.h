/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include "SceneNode.h"
#include "TextLayout.h"
#include "FontResource.h"

namespace ls {

class Texture;
class Style;

class TextSceneNode : public Napi::SafeObjectWrap<TextSceneNode>, public SceneNode {
 public:
    explicit TextSceneNode(const Napi::CallbackInfo& info);

    void OnPropertyChanged(StyleProperty property) override;

    void BeforeLayout() override;
    void AfterLayout() override;
    void Paint(PaintContext* paint) override;
    void Composite(CompositeContext* composite) override;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    Napi::Value GetText(const Napi::CallbackInfo& info);
    void SetText(const Napi::CallbackInfo& info, const Napi::Value& value);

 private:
    bool SetFont(Style* style);
    void ClearFont() noexcept;
    void DestroyRecursive() override;
    void AppendChild(SceneNode* child) override;
    YGSize Measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);
    void QueueTextLayout() noexcept;

 private:
    std::string text;
    ResourceLink<FontResource> font;
    TextLayout layout;

    friend Napi::SafeObjectWrap<TextSceneNode>;
};

} // namespace ls
