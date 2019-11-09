/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "SceneNode.h"
#include "TextLayout.h"
#include "FontResource.h"
#include <napi.h>

namespace ls {

class Texture;
class Style;

class TextSceneNode : public Napi::ObjectWrap<TextSceneNode>, public SceneNode {
 public:
    explicit TextSceneNode(const Napi::CallbackInfo& info);
    virtual ~TextSceneNode() = default;

    static Napi::Function Constructor(Napi::Env env);

    Napi::Value GetText(const Napi::CallbackInfo& info);
    void SetText(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Reference<Napi::Object>* AsReference() noexcept override { return this; }
    void OnPropertyChanged(StyleProperty property) override;

    void BeforeLayout() override;
    void AfterLayout() override;
    void Paint(Renderer* renderer) override;
    void Composite(CompositeContext* context) override;

 private:
    bool SetFont(Style* style);
    void ClearFont() noexcept;
    void DestroyRecursive() override;
    void AppendChild(SceneNode* child) override;
    YGSize Measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);

 private:
    std::string text;
    ResourceLink<FontResource> font;
    TextLayout layout;
};

} // namespace ls
