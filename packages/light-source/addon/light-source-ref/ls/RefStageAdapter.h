/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <unordered_map>
#include <napi.h>
#include <ls/StageAdapter.h>

namespace ls {

class RefStageAdapter : public Napi::SafeObjectWrap<RefStageAdapter>, public StageAdapter {
 public:
    explicit RefStageAdapter(const Napi::CallbackInfo& info);

    std::unique_ptr<SceneAdapter> CreateSceneAdapter(const SceneAdapterConfig& config) override;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    Napi::Value GetKeyboard(const Napi::CallbackInfo& info) override;
    Napi::Value GetGamepads(const Napi::CallbackInfo& info) override;
    Napi::Value GetDisplays(const Napi::CallbackInfo& info) override;
    Napi::Value ProcessEvents(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    void Destroy(const Napi::CallbackInfo& info) override;
    void SetCallback(const Napi::CallbackInfo& info) override;
    void ResetCallbacks(const Napi::CallbackInfo& info) override;
};

} // namespace ls

#undef DeclareStageCallback
