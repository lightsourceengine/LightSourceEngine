/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/Rect.h>
#include <ls/ResourceProgress.h>
#include <ls/types.h>
#include <ls/SceneNode.h>
#include <napi-ext.h>

namespace ls {

class ImageSceneNode final : public Napi::SafeObjectWrap<ImageSceneNode>, public SceneNode {
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

    void OnStylePropertyChanged(StyleProperty property) override;
    void OnBoundingBoxChanged() override;
    void OnStyleLayout() override;
    YGSize OnMeasure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;

    void Paint(RenderingContext2D* context) override;
    void Composite(CompositeContext* composite) override;
    void Destroy() override;

 private:
    void ClearResource();

 private:
    std::string src;
    ImageRef image{};
    ImageRect imageRect{};
    ResourceProgress resourceProgress;
};

} // namespace ls
