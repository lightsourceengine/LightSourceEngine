/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLTexture.h"
#include "SDLRenderer.h"
#include <ls/Surface.h>
#include <ls/Log.h>
#include <ls/Timer.h>

namespace ls {

static SDL_Texture* CreateTexture(SDLRenderer* renderer, const int32_t width, const int32_t height,
    const SDL_TextureAccess access) noexcept;

SDLTexture::SDLTexture(SDLRenderer* renderer, const int32_t width, const int32_t height,
        const SDL_TextureAccess access) noexcept : width(width), height(height), access(access) {
    this->texture = CreateTexture(renderer, width, height, access);
    this->format = renderer->GetTextureFormat();
}

SDLTexture::SDLTexture(SDLRenderer* renderer, const Surface& surface, const SDL_TextureAccess access) noexcept
: width(surface.Width()), height(surface.Height()), access(access) {
    if (surface.IsEmpty()
            || (surface.Format() == PixelFormatAlpha && access != SDL_TEXTUREACCESS_STREAMING)
            || (surface.Format() != renderer->GetTextureFormat())) {
        LOG_ERROR("Incompatible surface");
        return;
    }

    this->texture = CreateTexture(renderer, this->width, this->height, access);
    this->format = renderer->GetTextureFormat();

    if (surface.Format() == PixelFormatAlpha) {
        auto textureSurface{ this->Lock() };

        textureSurface.Blit(0, 0, surface);
    } else if (this->Update(surface)) {
        return;
    } else {
        SDL_DestroyTexture(this->texture);
        this->texture = nullptr;
    }
}

SDLTexture::~SDLTexture() noexcept {
    if (this->texture) {
        SDL_DestroyTexture(this->texture);
        this->texture = nullptr;
    }
}

bool SDLTexture::IsRenderTarget() const noexcept {
    return this->access == SDL_TEXTUREACCESS_TARGET;
}

bool SDLTexture::IsAttached() const noexcept {
    return this->texture != nullptr;
}

bool SDLTexture::CanUpdate() const noexcept {
    return this->access != SDL_TEXTUREACCESS_TARGET;
}

bool SDLTexture::Update(const Surface& surface) noexcept {
    if (!this->IsAttached() || !this->CanUpdate()) {
        LOG_ERROR("Not supported");
        return false;
    }

    if (surface.IsEmpty() || surface.Format() != this->format) {
        LOG_ERROR("Incompatible surface");
        return false;
    }

    const Timer t("SDL_UpdateTexture");

    if (SDL_UpdateTexture(this->texture, nullptr, surface.Pixels(), surface.Pitch()) != 0) {
        LOG_ERROR(SDL_GetError());
        return false;
    }

    return true;
}


bool SDLTexture::CanLock() const noexcept {
    return this->access == SDL_TEXTUREACCESS_STREAMING;
}

Surface SDLTexture::Lock() noexcept {
    if (!this->IsAttached() || !this->CanLock()) {
        LOG_ERROR("Not supported");
        return {};
    }

    void* pixels{nullptr};
    int32_t pitch{0};

    if (SDL_LockTexture(this->texture, nullptr, &pixels, &pitch) != 0) {
        LOG_ERROR(SDL_GetError());
        return {};
    }

    auto deleter = [&](uint8_t* data) {
        SDL_UnlockTexture(this->texture);
    };

    return {
        std::shared_ptr<uint8_t>(reinterpret_cast<uint8_t*>(pixels), deleter),
        this->width,
        this->height,
        pitch,
        this->format
    };
}

static SDL_Texture* CreateTexture(SDLRenderer* renderer, const int32_t width, const int32_t height,
        const SDL_TextureAccess access) noexcept {
    if (!renderer || !renderer->ToRawRenderer()) {
        return nullptr;
    }

    auto result = SDL_CreateTexture(
        renderer->ToRawRenderer(),
        renderer->GetRawTextureFormat(),
        access,
        width,
        height);

    if (result) {
        SDL_SetTextureBlendMode(result, SDL_BLENDMODE_BLEND);
        LOG_INFO("%ix%ix32", width, height);
    } else {
        LOG_ERROR(SDL_GetError());
    }

    return result;
}

} // namespace ls
