/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <memory>
#include <algorithm>
#include "ImageStore.h"
#include "LayerCache.h"

namespace ls {

class SceneNode;
class Stage;
class SceneAdapter;
class Renderer;

class Scene : public Napi::ObjectWrap<Scene> {
 public:
    explicit Scene(const Napi::CallbackInfo& info);
    virtual ~Scene() = default;

    static Napi::Function Constructor(Napi::Env env);

    int32_t GetWidth() const noexcept { return this->width; }
    int32_t GetHeight() const noexcept { return this->height; }
    int32_t GetViewportMin() const noexcept { return std::min(this->width, this->height); }
    int32_t GetViewportMax() const noexcept { return std::max(this->width, this->height); }
    int32_t GetRootFontSize() const noexcept { return this->rootFontSize; }

    LayerCache* GetLayerCache() const noexcept { return &this->layerCache; }
    ImageStore* GetImageStore() const noexcept { return &this->imageStore; }
    Renderer* GetRenderer() const noexcept;

    Stage* GetStage() const { return this->stage; }
    void NotifyRootFontSizeChanged(int32_t rootFontSize);
    void SetActiveNode(Napi::Value node);

 private: // javascript bindings
    void Attach(const Napi::CallbackInfo& info);
    void Detach(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);
    void Resize(const Napi::CallbackInfo& info);
    void Frame(const Napi::CallbackInfo& info);
    Napi::Value GetStage(const Napi::CallbackInfo& info);
    Napi::Value GetTitle(const Napi::CallbackInfo& info);
    void SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value);

 private:
    mutable ImageStore imageStore;
    mutable LayerCache layerCache;
    SceneNode* root{};
    Stage* stage{};
    std::unique_ptr<SceneAdapter> adapter;
    int32_t rootFontSize{0};
    int32_t width{};
    int32_t height{};
    bool isSizeDirty{true};
    bool isRootFontSizeDirty{true};
    bool isAttached{false};
};

} // namespace ls
