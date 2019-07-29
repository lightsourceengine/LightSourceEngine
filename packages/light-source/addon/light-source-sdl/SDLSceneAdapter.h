/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <SceneAdapter.h>
#include <SDL.h>

namespace ls {

class Renderer;

class SDLSceneAdapter : public SceneAdapter {
 public:
    explicit SDLSceneAdapter(int32_t displayId);
    virtual ~SDLSceneAdapter() = default;

    void Attach() override;
    void Detach() override;
    void Resize(int32_t width, int32_t height, bool fullscreen) override;
    Renderer* GetRenderer() const override;

 private:
    SDL_Window* window{};
    Renderer* renderer{};
};

} // namespace ls
