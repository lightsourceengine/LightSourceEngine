/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "SDLRenderer.h"
#include <SceneAdapter.h>
#include <SDL.h>
#include <memory>

namespace ls {

class SDLSceneAdapter : public SceneAdapter {
 public:
    SDLSceneAdapter();
    virtual ~SDLSceneAdapter() = default;

    void Attach() override;
    void Detach() override;
    void Resize(int32_t width, int32_t height, bool fullscreen) override;

    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    bool GetFullscreen() const override;
    Renderer* GetRenderer() const override;

 private:
    std::unique_ptr<SDLRenderer> renderer{};
    SDL_Window* window{};
    int32_t width{};
    int32_t height{};
    bool fullscreen{};
};

} // namespace ls
