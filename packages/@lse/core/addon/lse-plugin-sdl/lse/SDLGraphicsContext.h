/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/SDL2.h>
#include <lse/GraphicsContext.h>

namespace lse {

class SDLRenderer;

/**
 * SDL GraphicsContext implementation.
 */
class SDLGraphicsContext final : public GraphicsContext {
 public:
  SDLGraphicsContext(const GraphicsContextConfig& config);

  void Attach() override;
  void Detach() override;

  void SetTitle(const char* title) override;
  const char* GetTitle() const noexcept override;

 private:
  SDL_Window* window{};
  std::shared_ptr<SDLRenderer> sdlRenderer;
};

} // namespace lse
