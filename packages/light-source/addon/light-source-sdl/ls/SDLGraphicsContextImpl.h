/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <SDL.h>
#include <ls/SDLRenderer.h>
#include <ls/GraphicsContext.h>

namespace ls {

class SDLGraphicsContextImpl final : public GraphicsContextInterface {
 public:
    explicit SDLGraphicsContextImpl(const Napi::CallbackInfo& info);
    virtual ~SDLGraphicsContextImpl() = default;

    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    void Resize(const Napi::CallbackInfo& info) override;
    Napi::Value GetTitle(const Napi::CallbackInfo& info) override;
    void SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value) override;
    Napi::Value GetWidth(const Napi::CallbackInfo& info) override;
    Napi::Value GetHeight(const Napi::CallbackInfo& info) override;
    Napi::Value GetDisplayIndex(const Napi::CallbackInfo& info) override;
    Napi::Value GetFullscreen(const Napi::CallbackInfo& info) override;

    int32_t GetWidth() const override { return this->width; }
    int32_t GetHeight() const override { return this->height; }
    Renderer* GetRenderer() const override;
    void Finalize() override;

 private:
    mutable SDLRenderer renderer;
    SDL_Window* window{};

    int32_t configWidth{};
    int32_t configHeight{};
    bool configFullscreen{};
    int32_t configDisplayIndex{};

    int32_t width{};
    int32_t height{};
    bool fullscreen{};
    std::string title{"Light Source App"};
};

} // namespace ls
