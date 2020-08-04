/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include "SceneNode.h"
#include "Resources.h"
#include <ls/Rect.h>

namespace ls {

class ImageSceneNode : public Napi::SafeObjectWrap<ImageSceneNode>, public SceneNode {
 public:
    explicit ImageSceneNode(const Napi::CallbackInfo& info);

    void OnPropertyChanged(StyleProperty property) override;
    void BeforeLayout() override;
    void AfterLayout() override;
    void Paint(PaintContext* paint) override;
    void Composite(CompositeContext* composite) override;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    Napi::Value GetSource(const Napi::CallbackInfo& info);
    void SetSource(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetOnLoadCallback(const Napi::CallbackInfo& info);
    void SetOnLoadCallback(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetOnErrorCallback(const Napi::CallbackInfo& info);
    void SetOnErrorCallback(const Napi::CallbackInfo& info, const Napi::Value& value);

 private:
    void DestroyRecursive() override;
    void DoCallbacks();
    void AppendChild(SceneNode* child) override;
    void ClearResource();

 private:
    std::string src;
    ImageData* image{};
    Napi::FunctionReference onLoadCallback;
    Napi::FunctionReference onErrorCallback;
    Rect destRect{};
    Rect srcRect{};

    friend Napi::SafeObjectWrap<ImageSceneNode>;
};

} // namespace ls
