/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "RefRenderer.h"
#include <ls/SceneAdapter.h>
#include <memory>

namespace ls {

class RefSceneAdapter : public Napi::SafeObjectWrap<RefSceneAdapter>, public SceneAdapter {
 public:
    explicit RefSceneAdapter(const Napi::CallbackInfo& info);

    static Napi::Function GetClass(Napi::Env env);

    void Constructor(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    void Resize(const Napi::CallbackInfo& info) override;
    Napi::Value GetTitle(const Napi::CallbackInfo& info) override;
    void SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value) override;
    Napi::Value GetWidth(const Napi::CallbackInfo& info) override;
    Napi::Value GetHeight(const Napi::CallbackInfo& info) override;
    Napi::Value GetFullscreen(const Napi::CallbackInfo& info) override;
    Napi::Value GetDisplayIndex(const Napi::CallbackInfo& info) override;

    int32_t GetWidth() const override { return this->width; }
    int32_t GetHeight() const override { return this->height; }
    Renderer* GetRenderer() const override;

 private:
    mutable RefRenderer renderer;
    std::string title{};
    int32_t width{};
    int32_t height{};
    int32_t displayIndex{};
    bool fullscreen{};
};

} // namespace ls
