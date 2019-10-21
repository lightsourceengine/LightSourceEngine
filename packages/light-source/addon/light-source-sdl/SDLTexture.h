/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <SDL.h>
#include <ls/Texture.h>

namespace ls {

class SDLRenderer;
class Surface;

class SDLTexture : public Texture {
 public:
    SDLTexture(SDLRenderer* renderer, const int32_t width, const int32_t height,
        const SDL_TextureAccess access) noexcept;
    SDLTexture(SDLRenderer* renderer, const Surface& surface, const SDL_TextureAccess access) noexcept;
    virtual ~SDLTexture() noexcept;

    int32_t GetWidth() const noexcept override { return this->width; }
    int32_t GetHeight() const noexcept override { return this->height; }
    PixelFormat GetFormat() const noexcept override { return this->format; }
    bool IsRenderTarget() const noexcept override;
    bool IsAttached() const noexcept override;
    bool CanUpdate() const noexcept override;
    bool Update(const Surface& surface) noexcept override;
    bool CanLock() const noexcept override;
    Surface Lock() noexcept override;

    SDL_Texture* ToRawTexture() noexcept {
        return this->texture;
    }

 private:
    int32_t width;
    int32_t height;
    SDL_TextureAccess access;
    PixelFormat format{PixelFormatUnknown};
    SDL_Texture* texture{nullptr};
};

} // namespace ls
