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

 private:
    ResourceManager* resourceManager{};
    std::shared_ptr<SceneAdapter> adapter;
};

} // namespace ls
