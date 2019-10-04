/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLSceneAdapter.h"
#include <fmt/println.h>

namespace ls {

SDLSceneAdapter::SDLSceneAdapter(const SceneAdapterConfig& config) : config(config) {
}

SDLSceneAdapter::~SDLSceneAdapter() {
    this->renderer.Destroy();

    if (this->window) {
        SDL_DestroyWindow(this->window);
    }
}

void SDLSceneAdapter::Attach() {
    if (!this->window) {
        auto displayIndex{ this->config.displayIndex };

        this->window = SDL_CreateWindow(
            this->title.c_str(),
            SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
            SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
            this->config.width,
            this->config.height,
            this->config.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

        if (!this->window) {
            throw std::runtime_error(fmt::format("Failed to create an SDL window. SDL Error: {}", SDL_GetError()));
        }

        if (this->config.fullscreen) {
            SDL_ShowCursor(SDL_DISABLE);
        }

        SDL_DisplayMode displayMode{};

        SDL_GetWindowDisplayMode(window, &displayMode);

        fmt::println("Window: size={},{} format={} refreshRate={} displayIndex={}",
            displayMode.w,
            displayMode.h,
            SDL_GetPixelFormatName(displayMode.format),
            displayMode.refresh_rate,
            displayIndex);
    }

    this->renderer.Attach(this->window);

    this->width = this->renderer.GetWidth();
    this->height = this->renderer.GetHeight();
    this->fullscreen = false;
}

void SDLSceneAdapter::Detach() {
    this->renderer.Detach();

    if (this->window) {
        SDL_DestroyWindow(this->window);
        this->window = nullptr;
    }
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
    return &this->renderer;
}

void SDLSceneAdapter::SetTitle(const std::string& title) {
    this->title = title;

    if (this->window) {
        SDL_SetWindowTitle(this->window, this->title.c_str());
    }
}

} // namespace ls
