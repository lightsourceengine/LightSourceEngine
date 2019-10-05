/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <memory>
#include <algorithm>

namespace ls {

class SceneNode;
class Stage;
class ResourceManager;
class SceneAdapter;

class Scene : public Napi::ObjectWrap<Scene> {
 public:
    explicit Scene(const Napi::CallbackInfo& info);
    virtual ~Scene() = default;

    // javascript methods

    static Napi::Function Constructor(Napi::Env env);
    void Attach(const Napi::CallbackInfo& info);
    void Detach(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);
    void Resize(const Napi::CallbackInfo& info);
    void Frame(const Napi::CallbackInfo& info);

    Napi::Value GetStage(const Napi::CallbackInfo& info);
    Napi::Value GetTitle(const Napi::CallbackInfo& info);
    void SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value);

    // native methods

    int32_t GetWidth() const { return this->width; }
    int32_t GetHeight() const { return this->height; }
    int32_t GetViewportMin() const { return std::min(this->width, this->height); }
    int32_t GetViewportMax() const { return std::max(this->width, this->height); }
    int32_t GetRootFontSize() const { return this->rootFontSize; }
    ResourceManager* GetResourceManager() const { return this->resourceManager; }
    Stage* GetStage() const { return this->stage; }
    void NotifyRootFontSizeChanged(int32_t rootFontSize);
    void SetActiveNode(Napi::Value node);

 private:
    ResourceManager* resourceManager{};
    SceneNode* root{};
    Stage* stage{};
    std::unique_ptr<SceneAdapter> adapter;
    int32_t rootFontSize{0};
    int32_t width{};
    int32_t height{};
    bool isSizeDirty{false};
    bool isRootFontSizeDirty{false};
    bool isAttached{false};
};

} // namespace ls
