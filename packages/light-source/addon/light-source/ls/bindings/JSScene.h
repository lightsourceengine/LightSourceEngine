/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/types.h>

namespace ls {
namespace bindings {

/**
 * Bindings for a javascript Scene object.
 */
class JSScene : public Napi::SafeObjectWrap<JSScene> {
 public:
    JSScene(const Napi::CallbackInfo& info);
    ~JSScene() override = default;

    static Napi::Function GetClass(Napi::Env env);
    SceneRef GetNative() const noexcept;

 private:
    void Constructor(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info);
    void Detach(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);
    void Frame(const Napi::CallbackInfo& info);
    Napi::Value SetRoot(const Napi::CallbackInfo& info);
    Napi::Value SetStage(const Napi::CallbackInfo& info);
    Napi::Value SetGraphicsContext(const Napi::CallbackInfo& info);

 private:
    SceneRef native{};

    friend Napi::SafeObjectWrap<JSScene>;
};

} // namespace bindings
} // namespace ls
