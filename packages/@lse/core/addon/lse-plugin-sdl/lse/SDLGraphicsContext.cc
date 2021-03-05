/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/SDLGraphicsContext.h>

#include <lse/SDLRenderer.h>
#include <lse/Log.h>
#include <lse/string-ext.h>

namespace lse {

static Uint32 GetFullscreenFlag(const GraphicsContextConfig& config) noexcept;

SDLGraphicsContext::SDLGraphicsContext(const GraphicsContextConfig& config) : GraphicsContext() {
  this->sdlRenderer = SDLRenderer::New();
  this->SetRenderer(this->sdlRenderer);
  this->SetConfig(config);
}

void SDLGraphicsContext::Attach() {
  if (!this->sdlRenderer) {
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
        GetFullscreenFlag(this->config));

    if (!this->window) {
      LOG_ERROR("SDL_CreateWindow(): %s", SDL2::SDL_GetError());
      throw std::runtime_error("Failed to create window");
    }

    isWindowFullscreen = (SDL2::SDL_GetWindowFlags(this->window)
        & (SDL_WINDOW_FULLSCREEN));

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

    this->refreshRate = displayMode.refresh_rate;
  }

  this->sdlRenderer->Attach(this->window);

  this->width = this->sdlRenderer->GetWidth();
  this->height = this->sdlRenderer->GetHeight();
  this->fullscreen = isWindowFullscreen;
  this->displayIndex = displayIndex;
}

void SDLGraphicsContext::Detach() {
  if (!this->sdlRenderer) {
    return;
  }

  this->sdlRenderer->Detach();

  if (this->window) {
    SDL2::SDL_DestroyWindow(this->window);
    this->window = nullptr;
    this->refreshRate = 0;
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

static Uint32 GetFullscreenFlag(const GraphicsContextConfig& config) noexcept {
  if (!config.fullscreen) {
    return 0;
  } else if (EqualsIgnoreCase(config.fullscreenMode, "exclusive")) {
    return SDL_WINDOW_FULLSCREEN;
  } else {
    std::string driver{SDL2::SDL_GetCurrentVideoDriver()};

    // default to exclusive on systems without a window manager
    if (EqualsIgnoreCase(driver, "rpi") || EqualsIgnoreCase(driver, "kmsdrm")) {
      return SDL_WINDOW_FULLSCREEN;
    }

    return SDL_WINDOW_FULLSCREEN_DESKTOP;
  }
}

} // namespace lse
