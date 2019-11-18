/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <memory>
#include <SafeObjectWrap.h>
#include "SceneAdapter.h"

namespace ls {

/**
 * Provides an interface for the Stage to communicate with the native window manager and graphics environment.
 */
class StageAdapter : public virtual Napi::SafeObjectWrapBase {
 public:
    // javascript methods

    virtual Napi::Value GetKeyboard(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetGamepads(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetDisplays(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value ProcessEvents(const Napi::CallbackInfo& info) = 0;
    virtual void Attach(const Napi::CallbackInfo& info) = 0;
    virtual void Detach(const Napi::CallbackInfo& info) = 0;
    virtual void Destroy(const Napi::CallbackInfo& info) = 0;
    virtual void SetCallback(const Napi::CallbackInfo& info) = 0;
    virtual void ResetCallbacks(const Napi::CallbackInfo& info) = 0;

    // native methods

    /**
     * Factory method for creating a SceneAdapter.
     */
    virtual std::unique_ptr<SceneAdapter> CreateSceneAdapter(const SceneAdapterConfig& config) = 0;
};

} // namespace ls
