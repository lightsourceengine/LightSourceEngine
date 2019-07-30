/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <memory>
#include "ResourceManager.h"
#include "SceneAdapter.h"

namespace ls {

class SceneNode;

class Scene : public Napi::ObjectWrap<Scene> {
 public:
    explicit Scene(const Napi::CallbackInfo& info);
    virtual ~Scene() = default;

    static Napi::Function Constructor(Napi::Env env);
    Napi::Value GetResourceManager(const Napi::CallbackInfo& info);
    void Attach(const Napi::CallbackInfo& info);
    void Detach(const Napi::CallbackInfo& info);
    void Resize(const Napi::CallbackInfo& info);
    void ProcessEvents(const Napi::CallbackInfo& info);

    // TODO: get w/h
    int32_t GetWidth() const { return 0; }
    int32_t GetHeight() const { return 0; }
    int32_t GetViewportMin() const { return 0; }
    int32_t GetViewportMax() const { return 0; }

    ResourceManager* GetResourceManager() { return this->resourceManager; }

 private:
    ResourceManager* resourceManager{};
    SceneNode* root;
    std::shared_ptr<SceneAdapter> adapter;
};

} // namespace ls
