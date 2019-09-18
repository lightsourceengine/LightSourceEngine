/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "SceneNode.h"
#include "TextBlock.h"
#include <napi.h>

namespace ls {

class FontResource;
class LayerResource;
class Font;

class TextSceneNode : public Napi::ObjectWrap<TextSceneNode>, public SceneNode {
 public:
    explicit TextSceneNode(const Napi::CallbackInfo& info);
    virtual ~TextSceneNode() = default;

    static Napi::Function Constructor(Napi::Env env);

    Napi::Value GetText(const Napi::CallbackInfo& info);
    void SetText(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Reference<Napi::Object>* AsReference() override { return this; }
    void Paint(Renderer* renderer) override;

    void OnViewportSizeChange() override;
    void OnRootFontSizeChange() override;

 private:
    void UpdateStyle(Style* newStyle, Style* oldStyle) override;
    bool SetFont(Style* style);
    void ClearFont();
    void DestroyRecursive() override;
    void AppendChild(SceneNode* child) override;
    YGSize Measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);

 private:
    std::string text;

    std::shared_ptr<FontResource> fontResource{};
    std::shared_ptr<Font> font;
    uint32_t fontResourceListenerId{};
    LayerResource* layer{};
    TextBlock textBlock;
};

} // namespace ls
