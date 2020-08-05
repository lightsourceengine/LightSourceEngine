/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/Renderer.h>

namespace ls {

class GraphicsContextInterface {
 public:
    virtual void Attach(const Napi::CallbackInfo& info) = 0;
    virtual void Detach(const Napi::CallbackInfo& info) = 0;
    virtual void Resize(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetTitle(const Napi::CallbackInfo& info) = 0;
    virtual void SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value) = 0;
    virtual Napi::Value GetWidth(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetHeight(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetFullscreen(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetDisplayIndex(const Napi::CallbackInfo& info) = 0;

    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
    virtual Renderer* GetRenderer() const = 0;

    virtual void Finalize() = 0;
};

using GraphicsContextInterfaceFactory = GraphicsContextInterface* (*)(const Napi::CallbackInfo& info);

class GraphicsContext : public Napi::SafeObjectWrap<GraphicsContext>, public GraphicsContextInterface {
 public:
    GraphicsContext(const Napi::CallbackInfo& info);
    ~GraphicsContext() override;

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
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    Renderer* GetRenderer() const override;
    void Finalize() override;

    static Napi::Function GetClass(Napi::Env env);
    template<typename T>
    static Napi::Value Create(Napi::Env env, Napi::Value config);

 private:
    GraphicsContextInterface* impl{};
};

template<typename T>
Napi::Value GraphicsContext::Create(Napi::Env env, Napi::Value config) {
    const GraphicsContextInterfaceFactory factory{
        [](const Napi::CallbackInfo& info) -> GraphicsContextInterface* {
          return new T(info);
        }
    };

    Napi::EscapableHandleScope scope(env);
    auto external{ Napi::External<void>::New(env, reinterpret_cast<void*>(factory)) };

    return scope.Escape(GraphicsContext::GetClass(env).New({ external, config }));
}

} // namespace ls
