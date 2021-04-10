/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include <lse/SDLUtil.h>
#include <lse/Renderer.h>

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
