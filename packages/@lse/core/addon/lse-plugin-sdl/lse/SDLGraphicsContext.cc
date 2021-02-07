/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/SDLGraphicsContext.h>

#include <lse/SDLRenderer.h>
#include <lse/Log.h>

namespace lse {

SDLGraphicsContext::SDLGraphicsContext(const GraphicsContextConfig& config) : GraphicsContext() {
  this->renderer = std::make_shared<SDLRenderer>();
  this->SetConfig(config);
}

void SDLGraphicsContext::Attach() {
  if (!this->renderer) {
    return;
  }

  bool isWindowFullscreen{ false };

  if (!this->window) {
    auto displayIndex{ this->config.displayIndex };

    this->window = SDL2::SDL_CreateWindow(
        this->title.c_str(),
        SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
        SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
        this->config.width,
        this->config.height,
        this->config.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

    if (!this->window) {
      LOG_ERROR("SDL_CreateWindow(): %s", SDL2::SDL_GetError());
      throw std::runtime_error("Failed to create window");
    }

    isWindowFullscreen = (SDL2::SDL_GetWindowFlags(this->window)
        & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP));

    if (isWindowFullscreen) {
      // TODO: if mouse events are ever supported, this should be configurable.
      SDL2::SDL_ShowCursor(SDL_DISABLE);
    }

    SDL_DisplayMode displayMode{};

    SDL2::SDL_GetWindowDisplayMode(this->window, &displayMode);

    LOGX_INFO("SDL_Window: %ix%i @ %ihz, format=%s, fullscreen=%s",
              displayMode.w,
              displayMode.h,
              displayMode.refresh_rate,
              SDL2::SDL_GetPixelFormatName(displayMode.format),
              isWindowFullscreen);
  }

  std::static_pointer_cast<SDLRenderer>(this->renderer)->Attach(this->window);

  this->width = this->renderer->GetWidth();
  this->height = this->renderer->GetHeight();
  this->fullscreen = isWindowFullscreen;
  this->displayIndex = displayIndex;
}

void SDLGraphicsContext::Detach() {
  if (!this->renderer) {
    return;
  }

  std::static_pointer_cast<SDLRenderer>(this->renderer)->Detach();

  if (this->window) {
    SDL2::SDL_DestroyWindow(this->window);
    this->window = nullptr;
  }
}

void SDLGraphicsContext::SetTitle(const char* title) {
  if (this->window) {
    SDL2::SDL_SetWindowTitle(this->window, title ? title : "");
  }

  GraphicsContext::SetTitle(title);
}

const char* SDLGraphicsContext::GetTitle() const noexcept {
  return GraphicsContext::GetTitle();
}

} // namespace lse
