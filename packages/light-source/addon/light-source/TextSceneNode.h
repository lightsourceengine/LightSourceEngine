/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "SceneNode.h"
#include "TextBlock.h"

namespace ls {

class FontResource;

class TextSceneNode : public Napi::ObjectWrap<TextSceneNode>, public SceneNode {
 public:
    explicit TextSceneNode(const Napi::CallbackInfo& info);
    virtual ~TextSceneNode() = default;

    static Napi::Function Constructor(Napi::Env env);

    Napi::Value GetText(const Napi::CallbackInfo& info);
    void SetText(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Reference<Napi::Object>* AsReference() override { return this; }
    void Paint(Renderer* renderer) override;

 private:
    void ApplyStyle(Style* style) override;
    void SetFontResource(FontResource* newFontResource);
    void DestroyRecursive() override;
    void AppendChild(SceneNode* child) override;
    std::string ApplyTextTransform(const std::string& text);
    std::string ApplyTextTransform(Napi::String text);

 private:
    std::string text;
    StyleTextTransform textTransform{StyleTextTransformNone};
    TextBlock textBlock;
    FontResource* fontResource{};
    uint32_t fontResourceListenerId{};
};

} // namespace ls
