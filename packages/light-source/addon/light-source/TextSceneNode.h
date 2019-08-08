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

class FontSampleResource;

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
    bool SetFont(FontSampleResource* newFont);
    void DestroyRecursive() override;
    void AppendChild(SceneNode* child) override;

 private:
    TextBlock textBlock;
    FontSampleResource* font{};
    uint32_t fontListenerId{};
};

} // namespace ls
