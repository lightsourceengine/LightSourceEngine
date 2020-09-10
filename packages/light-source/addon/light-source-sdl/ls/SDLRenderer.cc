/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/SDLRenderer.h>

#include <array>
#include <cstring>
#include <ls/SDLUtil.h>
#include <ls/PixelConversion.h>
#include <ls/Log.h>
#include <ls/string-ext.h>

namespace ls {

static const std::array<uint8_t, 4> kSinglePixelWhite{ 255, 255, 255, 255 };

SDLRenderer::SDLRenderer() {
    SDL_RendererInfo info;

    if (SDL2::SDL_GetRenderDriverInfo(0, &info) == 0) {
        this->UpdateTextureFormats(info);
    }
}

SDLRenderer::~SDLRenderer() {
    this->Destroy();
}

void SDLRenderer::UpdateTextureFormats(const SDL_RendererInfo& info) noexcept {
    constexpr static std::array<uint32_t, 4> supportedPixelFormats{
        SDL_PIXELFORMAT_ARGB8888,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_PIXELFORMAT_BGRA8888
    };

    uint32_t format{ SDL_PIXELFORMAT_UNKNOWN };

    for (auto &p : supportedPixelFormats) {
        for (auto i{ 0u }; i < info.num_texture_formats; i++) {
            if (p == info.texture_formats[i]) {
                format = p;
                break;
            }
        }
    }

    this->textureFormat = ToPixelFormat(format);
}

void SDLRenderer::Present() {
    SDL2::SDL_RenderPresent(this->renderer);
}

void SDLRenderer::DrawFillRect(const Rect& rect, const Matrix& transform, const color_t fillColor) {
    ls::DrawImage(this->renderer, this->fillRectTexture.Cast<SDL_Texture>(), rect, transform, fillColor);
}

void SDLRenderer::DrawImage(const Texture& texture, const Rect& rect,
        const Matrix& transform, color_t tintColor) {
    ls::DrawImage(this->renderer, texture.Cast<SDL_Texture>(), rect, transform, tintColor);
}

void SDLRenderer::DrawImage(const Texture& texture, const Rect& srcRect, const Rect& destRect,
        const Matrix& transform, color_t tintColor) {
    ls::DrawImage(this->renderer, texture.Cast<SDL_Texture>(), srcRect, destRect, transform, tintColor);
}

void SDLRenderer::DrawBorder(const Rect& rect, const EdgeRect& border, const Matrix& transform, color_t fillColor) {
    ls::DrawBorder(this->renderer, this->fillRectTexture.Cast<SDL_Texture>(), rect, border, transform, fillColor);
}

void SDLRenderer::DrawImage(const Texture& texture, const EdgeRect& capInsets, const Rect& rect,
        const Matrix& transform, color_t tintColor) {
    ls::DrawImage(this->renderer, texture.Cast<SDL_Texture>(), capInsets, rect, transform, tintColor);
}

void SDLRenderer::FillRenderTarget(const color_t color) {
    SetRenderDrawColor(color);
    SDL2::SDL_RenderClear(this->renderer);
}

bool SDLRenderer::SetRenderTarget(const Texture& newRenderTarget) {
    // TODO: exceptions?

    if (!newRenderTarget.IsRenderTarget()) {
        return false;
    }

    if (SDL2::SDL_SetRenderTarget(this->renderer, newRenderTarget.Cast<SDL_Texture>()) != 0) {
        LOG_ERROR(SDL2::SDL_GetError());

        return false;
    }

    this->ResetInternal(newRenderTarget);

    return true;
}


void SDLRenderer::Reset() {
    if (this->renderer) {
        SDL2::SDL_SetRenderTarget(this->renderer, nullptr);
    }

    this->ResetInternal({});
}

void SDLRenderer::ResetInternal(const Texture& newRenderTarget) {
    if (newRenderTarget.IsRenderTarget()) {
        this->renderTarget = newRenderTarget;
    } else {
        this->renderTarget = {};
    }

    this->DisableClipping();
    this->SetRenderDrawColor(ColorWhite);
    SDL2::SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
}

void SDLRenderer::EnabledClipping(const Rect& rect) {
    const auto clipRect { ToSDLRect(rect) };

    SDL2::SDL_RenderSetClipRect(this->renderer, &clipRect);
}

void SDLRenderer::DisableClipping() {
    SDL2::SDL_RenderSetClipRect(this->renderer, nullptr);
}

Texture SDLRenderer::CreateTexture(int32_t width, int32_t height, Texture::Type type) {
    return ls::CreateTexture(this->renderer, width, height, type, this->textureFormat);
}

void SDLRenderer::SetRenderDrawColor(color_t color) noexcept {
    if (this->drawColor != color) {
        // TODO: consider opacity
        SDL2::SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
        this->drawColor = color;
    }
}

void SDLRenderer::Attach(SDL_Window* window) {
    auto driverIndex{ 0 };

    this->renderer = SDL2::SDL_CreateRenderer(
        window, driverIndex, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!this->renderer) {
        throw std::runtime_error(Format("Failed to create an SDL renderer. SDL Error: %s", SDL2::SDL_GetError()));
    }

    this->fillRectTexture = ls::CreateTexture(this->renderer, 1, 1, Texture::Updatable, this->textureFormat);
    this->fillRectTexture.Update(kSinglePixelWhite.data(), kSinglePixelWhite.size() * sizeof(uint8_t));

    // TODO: check texture

    SDL2::SDL_GetRendererOutputSize(renderer, &this->width, &this->height);

    SDL_RendererInfo info{};

    if (SDL2::SDL_GetRenderDriverInfo(driverIndex, &info) == 0) {
        this->UpdateTextureFormats(info);
    }

    LOG_INFO("Renderer Info: size=%i,%i driver=%s textureFormat=%s maxTextureSize=%i,%i "
            "software=%s accelerated=%s vsync=%s renderTarget=%s",
        this->GetWidth(),
        this->GetHeight(),
        SDL2::SDL_GetVideoDriver(driverIndex),
        SDL2::SDL_GetPixelFormatName(ToSDLPixelFormat(this->textureFormat)),
        info.max_texture_width,
        info.max_texture_height,
        (info.flags & SDL_RENDERER_SOFTWARE) != 0,
        (info.flags & SDL_RENDERER_ACCELERATED) != 0,
        (info.flags & SDL_RENDERER_PRESENTVSYNC) != 0,
        (info.flags & SDL_RENDERER_TARGETTEXTURE) != 0);
}

void SDLRenderer::Detach() {
    this->fillRectTexture.Destroy();
    this->renderTarget = {};
    this->renderer = DestroyRenderer(this->renderer);
    this->width = 0;
    this->height = 0;
}

void SDLRenderer::Destroy() {
    this->Detach();
}

} // namespace ls
