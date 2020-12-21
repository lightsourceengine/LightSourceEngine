/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/Rect.h>
#include <lse/ResourceProgress.h>
#include <lse/types.h>
#include <lse/SceneNode.h>
#include <napi-ext.h>

namespace lse {

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

} // namespace lse
