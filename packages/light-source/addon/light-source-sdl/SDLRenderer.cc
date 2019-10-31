/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLRenderer.h"
#include "SDLTexture.h"
#include "SDLUtil.h"
#include <array>
#include <ls/PixelConversion.h>
#include <ls/Log.h>
#include <ls/Format.h>

namespace ls {

static void SetTextureTintColor(SDL_Texture* texture, const uint32_t color) noexcept;
static SDL_Texture* ToRawTexture(const std::shared_ptr<Texture>& texture) noexcept;
static SDL_Rect ToSDLRect(const Rect& rect) noexcept;

SDLRenderer::SDLRenderer() {
    SDL_RendererInfo info;

    if (SDL_GetRenderDriverInfo(0, &info) == 0) {
        this->UpdateTextureFormats(info);
    }
}

SDLRenderer::~SDLRenderer() {
    this->Destroy();
}

void SDLRenderer::UpdateTextureFormats(const SDL_RendererInfo& info) noexcept {
    constexpr static std::array<Uint32, 4> supportedPixelFormats{
        SDL_PIXELFORMAT_ARGB8888,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_PIXELFORMAT_BGRA8888
    };

    this->sdlTextureFormat = SDL_PIXELFORMAT_UNKNOWN;

    for (auto &p : supportedPixelFormats) {
        for (auto i{ 0u }; i < info.num_texture_formats; i++) {
            if (p == info.texture_formats[i]) {
                this->sdlTextureFormat = p;
                break;
            }
        }
    }

    this->textureFormat = ToPixelFormat(this->sdlTextureFormat);
}

void SDLRenderer::Present() {
    SDL_RenderPresent(this->renderer);
}

void SDLRenderer::DrawFillRect(const Rect& rect, const uint32_t fillColor) {
    if (!this->fillRectTexture) {
        return;
    }

    auto texturePtr{ ToRawTexture(this->fillRectTexture) };
    const SDL_Rect destRect{ ToSDLRect(rect) };

    SetTextureTintColor(texturePtr, fillColor);

    SDL_RenderCopy(this->renderer, texturePtr, nullptr, &destRect);
}

void SDLRenderer::DrawBorder(const Rect& rect, const EdgeRect& border, const uint32_t borderColor) {
    const auto x{ static_cast<int32_t>(rect.x) };
    const auto y{ static_cast<int32_t>(rect.y) };
    const auto w{ static_cast<int32_t>(rect.width) };
    const auto h{ static_cast<int32_t>(rect.height) };

    const SDL_Rect rects[] {
        { x, y, w, border.top },
        { x, y + h - border.bottom, w, border.bottom },
        { x, y + border.top, border.left, h - border.top - border.bottom },
        {x + w - border.right, y + border.top, border.right, h - border.top - border.bottom }
    };

    this->SetRenderDrawColor(borderColor);

    SDL_RenderFillRects(this->renderer, rects, 4);
}

void SDLRenderer::DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const uint32_t tintColor) {
    if (!texture) {
        return;
    }

    const auto texturePtr{ ToRawTexture(texture) };
    const SDL_Rect destRect{ ToSDLRect(rect) };

    SetTextureTintColor(texturePtr, tintColor);

    SDL_RenderCopy(this->renderer, texturePtr, nullptr, &destRect);
}

void SDLRenderer::DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const EdgeRect& capInsets,
        const uint32_t tintColor) {
    if (!texture) {
        return;
    }

    const auto texturePtr{ ToRawTexture(texture) };
    const auto x{ static_cast<int32_t>(rect.x) };
    const auto y{ static_cast<int32_t>(rect.y) };
    const auto w{ static_cast<int32_t>(rect.width) };
    const auto h{ static_cast<int32_t>(rect.height) };

    int32_t textureWidth;
    int32_t textureHeight;
    SDL_Rect srcRect;
    SDL_Rect destRect;

    SetTextureTintColor(texturePtr, tintColor);
    SDL_QueryTexture(texturePtr, nullptr, nullptr, &textureWidth, &textureHeight);

    // Top row

    srcRect = { 0, 0, capInsets.left, capInsets.top };
    destRect = { x, y, capInsets.left, capInsets.top };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = { capInsets.left, 0, textureWidth - capInsets.left - capInsets.right, capInsets.top };
    destRect = {x + capInsets.left, y, w - capInsets.left - capInsets.right, capInsets.top };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = { textureWidth - capInsets.right, 0, capInsets.right, capInsets.top };
    destRect = {x + w - capInsets.right, y, capInsets.right, capInsets.top };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    // Middle row

    srcRect = { 0, capInsets.top, capInsets.left, textureHeight - capInsets.top - capInsets.bottom };
    destRect = { x, y + capInsets.top, capInsets.left, h - capInsets.top - capInsets.bottom };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = {
        capInsets.left,
        capInsets.top,
        textureWidth - capInsets.left - capInsets.right,
        textureHeight - capInsets.top - capInsets.bottom
    };
    destRect = {
        x + capInsets.left,
        y + capInsets.top,
        w - capInsets.left - capInsets.right,
        h - capInsets.top - capInsets.bottom
    };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = {
        textureWidth - capInsets.right,
        capInsets.top,
        capInsets.right,
        textureHeight - capInsets.top - capInsets.bottom
    };
    destRect = {
        x + w - capInsets.right,
        y + capInsets.top,
        capInsets.right,
        h - capInsets.top - capInsets.bottom
    };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    // Bottom row

    srcRect = { 0, textureHeight - capInsets.bottom, capInsets.left, capInsets.bottom };
    destRect = { x, y + h - capInsets.bottom, capInsets.left, capInsets.bottom };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = {
        capInsets.left,
        textureHeight - capInsets.bottom,
        textureWidth - capInsets.left - capInsets.right,
        capInsets.bottom
    };
    destRect = {
        x + capInsets.left,
        y + h - capInsets.bottom,
        w - capInsets.left - capInsets.right,
        capInsets.bottom
    };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = { textureWidth - capInsets.right, textureHeight - capInsets.bottom, capInsets.right, capInsets.bottom };
    destRect = {x + w - capInsets.right, y + h - capInsets.bottom, capInsets.right, capInsets.bottom };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);
}

void SDLRenderer::DrawQuad(const std::shared_ptr<Texture>& texture, const Rect& srcRect, const Rect& destRect,
        const uint32_t tintColor) {
    if (!texture) {
        return;
    }

    const auto texturePtr{ ToRawTexture(texture) };
    const SDL_Rect src{ ToSDLRect(srcRect) };
    const SDL_Rect dest{ ToSDLRect(destRect) };

    SetTextureTintColor(texturePtr, tintColor);

    SDL_RenderCopy(this->renderer, texturePtr, &src, &dest);
}

void SDLRenderer::FillRenderTarget(const uint32_t color) {
    SetRenderDrawColor(color);
    SDL_RenderClear(this->renderer);
}

bool SDLRenderer::SetRenderTarget(std::shared_ptr<Texture> renderTarget) {
    bool result{ true };

    if (renderTarget == nullptr) {
        SDL_SetRenderTarget(this->renderer, nullptr);
    } else if (SDL_SetRenderTarget(this->renderer, ToRawTexture(renderTarget)) != 0) {
        LOG_ERROR(SDL_GetError());
        renderTarget = nullptr;
        result = false;
    }

    this->renderTarget = renderTarget;
    this->ClearClipRect();
    this->SetRenderDrawColor(RGB(255, 255, 255));
    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);

    return result;
}

void SDLRenderer::SetClipRect(const Rect& rect) {
    const auto clipRect { ToSDLRect(rect) };

    SDL_RenderSetClipRect(this->renderer, &clipRect);
}

void SDLRenderer::ClearClipRect() {
    SDL_RenderSetClipRect(this->renderer, nullptr);
}

std::shared_ptr<Texture> SDLRenderer::CreateRenderTarget(const int32_t width, const int32_t height) {
    return std::static_pointer_cast<Texture>(std::make_shared<SDLTexture>(
        this, width, height, SDL_TEXTUREACCESS_TARGET));
}

std::shared_ptr<Texture> SDLRenderer::CreateTextureFromSurface(const Surface& surface) {
    return std::static_pointer_cast<Texture>(std::make_shared<SDLTexture>(this, surface, SDL_TEXTUREACCESS_STATIC));
}

std::shared_ptr<Texture> SDLRenderer::CreateTexture(const int32_t width, const int32_t height) {
    return std::static_pointer_cast<Texture>(std::make_shared<SDLTexture>(
        this, width, height, SDL_TEXTUREACCESS_STREAMING));
}

std::shared_ptr<Texture> SDLRenderer::CreateFillRectTexture() noexcept {
    // SDL2 FillRect API does not allow rotation. To work around the limitation, on attach, this texture is
    // created. A 2x2, solid white texture. The texture can be tinted to the desired fill rect color and rotated
    // through RenderCopyEx.
    constexpr auto FILL_RECT_TEXTURE_SIZE = 2;
    uint8_t rectTexturePixels[FILL_RECT_TEXTURE_SIZE * FILL_RECT_TEXTURE_SIZE * 4]{ 255 };
    Surface surface{
        std::shared_ptr<uint8_t>(rectTexturePixels, [](uint8_t*){}),
        FILL_RECT_TEXTURE_SIZE,
        FILL_RECT_TEXTURE_SIZE,
        0,
        this->GetTextureFormat()
    };

    try {
        return this->CreateTextureFromSurface(surface);
    } catch (const std::exception& e) {
        LOG_ERROR("Unable to create fill rect texture. %s", e);
    }

    return nullptr;
}

void SDLRenderer::SetRenderDrawColor(const uint32_t color) noexcept {
    if (this->drawColor != color) {
        // TODO: consider opacity
        SDL_SetRenderDrawColor(this->renderer, GetR(color), GetG(color), GetB(color), GetA(color));
        this->drawColor = color;
    }
}

void SDLRenderer::Attach(SDL_Window* window) {
    auto driverIndex{ 0 };

    this->renderer = SDL_CreateRenderer(window, driverIndex, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!this->renderer) {
        throw std::runtime_error(Format("Failed to create an SDL renderer. SDL Error: %s", SDL_GetError()));
    }

    this->fillRectTexture = this->CreateFillRectTexture();

    SDL_GetRendererOutputSize(renderer, &this->width, &this->height);

    SDL_RendererInfo info{};

    if (SDL_GetRenderDriverInfo(driverIndex, &info) == 0) {
        this->UpdateTextureFormats(info);
    }

    LOG_INFO("Renderer Info: size=%i,%i driver=%s textureFormat=%s maxTextureSize=%i,%i "
            "software=%s accelerated=%s vsync=%s renderTarget=%s",
        this->GetWidth(),
        this->GetHeight(),
        SDL_GetVideoDriver(driverIndex),
        SDL_GetPixelFormatName(this->sdlTextureFormat),
        info.max_texture_width,
        info.max_texture_height,
        (info.flags & SDL_RENDERER_SOFTWARE) != 0,
        (info.flags & SDL_RENDERER_ACCELERATED) != 0,
        (info.flags & SDL_RENDERER_PRESENTVSYNC) != 0,
        (info.flags & SDL_RENDERER_TARGETTEXTURE) != 0);
}

void SDLRenderer::Detach() {
    if (!this->renderer) {
        return;
    }

    this->fillRectTexture = nullptr;
    this->renderTarget = nullptr;

    SDL_DestroyRenderer(this->renderer);

    this->renderer = nullptr;
    this->width = 0;
    this->height = 0;
}

void SDLRenderer::Destroy() {
    this->Detach();
}

static void SetTextureTintColor(SDL_Texture* texture, const uint32_t color) noexcept {
    // TODO: consider opacity
    SDL_SetTextureColorMod(texture, GetR(color), GetG(color), GetB(color));
    SDL_SetTextureAlphaMod(texture, GetA(color));
}

static SDL_Texture* ToRawTexture(const std::shared_ptr<Texture>& texture) noexcept {
    if (texture) {
        return static_cast<SDLTexture*>(texture.get())->ToRawTexture();
    } else {
        return nullptr;
    }
}

static SDL_Rect ToSDLRect(const Rect& rect) noexcept {
    return {
        static_cast<int32_t>(rect.x),
        static_cast<int32_t>(rect.y),
        static_cast<int32_t>(rect.width),
        static_cast<int32_t>(rect.height),
    };
}

} // namespace ls
