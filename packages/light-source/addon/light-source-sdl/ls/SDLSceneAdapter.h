/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "SDLRenderer.h"
#include <ls/SceneAdapter.h>
#include <SDL.h>
#include <memory>

namespace ls {

class SDLSceneAdapter : public SceneAdapter {
 public:
    explicit SDLSceneAdapter(const SceneAdapterConfig& config);
    virtual ~SDLSceneAdapter();

    void Attach() override;
    void Detach() override;
    void Resize(int32_t width, int32_t height, bool fullscreen) override;

    void SetTitle(const std::string& title) override;
    std::string GetTitle() const override { return this->title; }
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    bool GetFullscreen() const override;
    Renderer* GetRenderer() const override;

 private:
    SceneAdapterConfig config;
    mutable SDLRenderer renderer;
    SDL_Window* window{};
    int32_t width{};
    int32_t height{};
    bool fullscreen{};
    std::string title{"Light Source App"};
};

} // namespace ls
