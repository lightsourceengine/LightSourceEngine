/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/SceneNode.h>
#include <ls/Resources.h>
#include <ls/Rect.h>

namespace ls {

class ImageSceneNode : public Napi::SafeObjectWrap<ImageSceneNode>, public SceneNode {
 public:
    ImageSceneNode(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<ImageSceneNode>(info) {}
    ~ImageSceneNode() override = default;

    static Napi::Function GetClass(Napi::Env env);
    void Constructor(const Napi::CallbackInfo& info) override;
    Napi::Value GetSource(const Napi::CallbackInfo& info);
    void SetSource(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetOnLoadCallback(const Napi::CallbackInfo& info);
    void SetOnLoadCallback(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetOnErrorCallback(const Napi::CallbackInfo& info);
    void SetOnErrorCallback(const Napi::CallbackInfo& info, const Napi::Value& value);

    void OnPropertyChanged(StyleProperty property) override;
    YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;

    void Paint(GraphicsContext* graphicsContext) override;
    void Composite(CompositeContext* composite) override;

 private:
    void DestroyRecursive() override;
    void ClearResource();

 private:
    std::string src;
    Image* image{};
    Napi::FunctionReference onLoadCallback;
    Napi::FunctionReference onErrorCallback;
    Rect destRect{};
    Rect srcRect{};
};

} // namespace ls
