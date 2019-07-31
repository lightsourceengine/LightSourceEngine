/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLSceneAdapter.h"
#include <fmt/format.h>

namespace ls {

SDLSceneAdapter::SDLSceneAdapter() : renderer(std::make_unique<SDLRenderer>()) {
}

void SDLSceneAdapter::Attach() {
    this->window = SDL_CreateWindow("Light Source App",
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), SDL_WINDOWPOS_CENTERED_DISPLAY(0), 1280, 720, 0);

    if (!this->window) {
        throw std::runtime_error(fmt::format("Failed to create an SDL window. SDL Error: {}", SDL_GetError()));
    }

    this->renderer->Attach(this->window);

    this->width = this->renderer->GetWidth();
    this->height = this->renderer->GetHeight();
    this->fullscreen = false;
}

void SDLSceneAdapter::Detach() {
    if (!this->window) {
        return;
    }

    this->renderer->Detach();

    SDL_DestroyWindow(this->window);
    this->window = nullptr;
}

void SDLSceneAdapter::Resize(int32_t width, int32_t height, bool fullscreen) {
}

int32_t SDLSceneAdapter::GetWidth() const {
    return this->width;
}

int32_t SDLSceneAdapter::GetHeight() const {
    return this->height;
}

bool SDLSceneAdapter::GetFullscreen() const {
    return this->fullscreen;
}

Renderer* SDLSceneAdapter::GetRenderer() const {
    return this->renderer.get();
}

} // namespace ls
