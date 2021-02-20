/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/SDLUtil.h>
#include <lse/Renderer.h>
#include <lse/Math.h>

namespace lse {

SDL_Renderer* DestroyRenderer(SDL_Renderer* renderer) noexcept {
  if (renderer) {
    SDL2::SDL_DestroyRenderer(renderer);
  }

  return nullptr;
}

void SDLSetDrawColor(SDL_Renderer* renderer, const RenderFilter& filter) noexcept {
  SDL2::SDL_SetRenderDrawColor(renderer, filter.tint.r, filter.tint.g, filter.tint.b, filter.tint.a);
}

void SDLSetTextureTint(SDL_Texture* texture, const RenderFilter& filter) noexcept {
  SDL2::SDL_SetTextureColorMod(texture, filter.tint.r, filter.tint.g, filter.tint.b);
  SDL2::SDL_SetTextureAlphaMod(texture, filter.tint.a);
}

SDL_RendererFlip SDLGetRenderFlip(const RenderFilter& filter) noexcept {
  int32_t flip{SDL_FLIP_NONE};

  if (filter.flipH) {
    flip |= SDL_FLIP_HORIZONTAL;
  }

  if (filter.flipV) {
    flip |= SDL_FLIP_VERTICAL;
  }

  return static_cast<SDL_RendererFlip>(flip);
}

} // namespace lse
