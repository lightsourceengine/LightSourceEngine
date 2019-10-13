/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "SceneNode.h"
#include "ImageResource.h"

namespace ls {

class ImageSceneNode : public Napi::ObjectWrap<ImageSceneNode>, public SceneNode {
 public:
    explicit ImageSceneNode(const Napi::CallbackInfo& info);
    virtual ~ImageSceneNode() = default;

    static Napi::Function Constructor(Napi::Env env);

    Napi::Value GetSource(const Napi::CallbackInfo& info);
    void SetSource(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Value GetOnLoadCallback(const Napi::CallbackInfo& info);
    void SetOnLoadCallback(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Value GetOnErrorCallback(const Napi::CallbackInfo& info);
    void SetOnErrorCallback(const Napi::CallbackInfo& info, const Napi::Value& value);

    void Paint(Renderer* renderer) override;

    Napi::Reference<Napi::Object>* AsReference() noexcept override { return this; }

 private:
    ImageUri uri{};
    ResourceLink<ImageResource> image;
    Napi::FunctionReference onLoadCallback;
    Napi::FunctionReference onErrorCallback;

 private:
    void DestroyRecursive() override;
    void DoCallbacks();
    void AppendChild(SceneNode* child) override;
};

} // namespace ls
