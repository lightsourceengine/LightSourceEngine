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
    auto displayIndex{ 0 };

    this->window = SDL_CreateWindow("Light Source App",
        SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex), 1280, 720, 0);

    if (!this->window) {
        throw std::runtime_error(fmt::format("Failed to create an SDL window. SDL Error: {}", SDL_GetError()));
    }

    SDL_DisplayMode displayMode{};

    SDL_GetWindowDisplayMode(window, &displayMode);

    fmt::println("Window: size={},{} format={} refreshRate={} displayIndex={}",
        displayMode.w,
        displayMode.h,
        SDL_GetPixelFormatName(displayMode.format),
        displayMode.refresh_rate,
        displayIndex);

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
