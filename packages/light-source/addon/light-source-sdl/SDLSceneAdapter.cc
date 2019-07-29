/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLSceneAdapter.h"
#include "SDLRenderer.h"

namespace ls {

SDLSceneAdapter::SDLSceneAdapter(int32_t displayId) {
    this->renderer = new SDLRenderer();
}

void SDLSceneAdapter::Attach() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
    SDL_GameControllerEventState(SDL_IGNORE);

    this->window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
}

void SDLSceneAdapter::Detach() {
}

void SDLSceneAdapter::Resize(int32_t width, int32_t height, bool fullscreen) {
}

Renderer* SDLSceneAdapter::GetRenderer() const {
    return this->renderer;
}

} // namespace ls
