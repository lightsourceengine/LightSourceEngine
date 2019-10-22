/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLRenderer.h"
#include "SDLTexture.h"
#include "SDLUtil.h"
#include <array>
#include <cassert>
#include <ls/PixelConversion.h>
#include <ls/Endian.h>
#include <ls/Log.h>
#include <ls/Format.h>
#include <ls/Log.h>

namespace ls {

static void SetTextureTintColor(SDL_Texture* texture, const int64_t color) noexcept;
static SDL_Texture* ToRawTexture(const std::shared_ptr<Texture>& texture) noexcept;

constexpr int64_t COLOR32{ 0xFFFFFFFF };

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
    static const std::array<Uint32, 4> supportedPixelFormats{
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

void SDLRenderer::Shift(float x, float y) {
    this->offsetStack.push_back(this->yOffset);
    this->offsetStack.push_back(this->xOffset);

    this->xOffset += x;
    this->yOffset += y;
}

void SDLRenderer::Unshift() {
    // TODO: validate stack!
    this->xOffset = this->offsetStack.back();
    this->offsetStack.pop_back();

    this->yOffset = this->offsetStack.back();
    this->offsetStack.pop_back();
}

void SDLRenderer::PushClipRect(const Rect& rect) {
    // TODO: round float values?
    SDL_Rect tempRect{
        static_cast<int32_t>(rect.x + this->xOffset),
        static_cast<int32_t>(rect.y + this->yOffset),
        static_cast<int32_t>(rect.width),
        static_cast<int32_t>(rect.height),
    };

    SDL_Rect* clipRect;

    if (!this->clipRectStack.empty()) {
        SDL_Rect intersect;
        auto back{ &this->clipRectStack.back() };

        clipRect = (SDL_TRUE == SDL_IntersectRect(back, &tempRect, &intersect)) ? &intersect : back;
    } else {
        clipRect = &tempRect;
    }

    this->clipRectStack.push_back(*clipRect);
    SDL_RenderSetClipRect(this->renderer, clipRect);
}

void SDLRenderer::PopClipRect() {
    // TODO: validate pop
//    if (this->clipRectStack.empty()) {
//        throw Error::New(info.Env(), "SDLRenderingContext.PopClipRect(): Clip rect stack should not be empty!");
//    }

    this->clipRectStack.pop_back();

    SDL_RenderSetClipRect(this->renderer, this->clipRectStack.empty() ? nullptr : &this->clipRectStack.back());
}

void SDLRenderer::DrawFillRect(const Rect& rect, const int64_t fillColor) {
    SDL_Rect tempRect{
        static_cast<int32_t>(rect.x + this->xOffset),
        static_cast<int32_t>(rect.y + this->yOffset),
        static_cast<int32_t>(rect.width),
        static_cast<int32_t>(rect.height),
    };

    this->SetRenderDrawColor(fillColor);

    SDL_RenderFillRect(this->renderer, &tempRect);
}

void SDLRenderer::DrawBorder(const Rect& rect, const EdgeRect& border, const int64_t borderColor) {
    auto x{ static_cast<int32_t>(rect.x + this->xOffset) };
    auto y{ static_cast<int32_t>(rect.y + this->yOffset) };
    auto width{ static_cast<int32_t>(rect.width) };
    auto height{ static_cast<int32_t>(rect.height) };

    SDL_Rect tempRect[] {
        { x, y, width, border.top },
        { x, y + height - border.bottom, width, border.bottom },
        { x, y + border.top, border.left, height - border.top - border.bottom },
        { x + width - border.right, y + border.top, border.right, height - border.top - border.bottom }
    };

    this->SetRenderDrawColor(borderColor);

    SDL_RenderFillRects(this->renderer, &tempRect[0], 4);
}

void SDLRenderer::DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const int64_t tintColor) {
    auto texturePtr{ ToRawTexture(texture) };

    if (!texturePtr) {
        return;
    }

    SDL_Rect tempRect{
        static_cast<int32_t>(rect.x + this->xOffset),
        static_cast<int32_t>(rect.y + this->yOffset),
        static_cast<int32_t>(rect.width),
        static_cast<int32_t>(rect.height),
    };

    SetTextureTintColor(texturePtr, tintColor);

    SDL_RenderCopy(this->renderer, texturePtr, nullptr, &tempRect);
}

void SDLRenderer::DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const EdgeRect& capInsets,
        const uint32_t tintColor) {
    auto texturePtr{ ToRawTexture(texture) };

    if (!texturePtr) {
        return;
    }

    auto x{ static_cast<int32_t>(rect.x + this->xOffset) };
    auto y{ static_cast<int32_t>(rect.y + this->yOffset) };
    auto width{ static_cast<int32_t>(rect.width) };
    auto height{ static_cast<int32_t>(rect.height) };

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
    destRect = { x + capInsets.left, y, width - capInsets.left - capInsets.right, capInsets.top };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = { textureWidth - capInsets.right, 0, capInsets.right, capInsets.top };
    destRect = { x + width - capInsets.right, y, capInsets.right, capInsets.top };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    // Middle row

    srcRect = { 0, capInsets.top, capInsets.left, textureHeight - capInsets.top - capInsets.bottom };
    destRect = { x, y + capInsets.top, capInsets.left, height - capInsets.top - capInsets.bottom };

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
        width - capInsets.left - capInsets.right,
        height - capInsets.top - capInsets.bottom
    };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = {
        textureWidth - capInsets.right,
        capInsets.top,
        capInsets.right,
        textureHeight - capInsets.top - capInsets.bottom
    };
    destRect = {
        x + width - capInsets.right,
        y + capInsets.top,
        capInsets.right,
        height - capInsets.top - capInsets.bottom
    };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    // Bottom row

    srcRect = { 0, textureHeight - capInsets.bottom, capInsets.left, capInsets.bottom };
    destRect = { x, y + height - capInsets.bottom, capInsets.left, capInsets.bottom };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = {
        capInsets.left,
        textureHeight - capInsets.bottom,
        textureWidth - capInsets.left - capInsets.right,
        capInsets.bottom
    };
    destRect = {
        x + capInsets.left,
        y + height - capInsets.bottom,
        width - capInsets.left - capInsets.right,
        capInsets.bottom
    };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);

    srcRect = { textureWidth - capInsets.right, textureHeight - capInsets.bottom, capInsets.right, capInsets.bottom };
    destRect = { x + width - capInsets.right, y + height - capInsets.bottom, capInsets.right, capInsets.bottom };

    SDL_RenderCopy(this->renderer, texturePtr, &srcRect, &destRect);
}

void SDLRenderer::DrawQuad(const std::shared_ptr<Texture>& texture, const Rect& srcRect, const Rect& destRect,
        const int64_t tintColor) {
    SDL_Texture* texturePtr{ ToRawTexture(texture) };

    if (!texturePtr) {
        return;
    }

    SDL_Rect src{
        static_cast<int32_t>(srcRect.x),
        static_cast<int32_t>(srcRect.y),
        static_cast<int32_t>(srcRect.width),
        static_cast<int32_t>(srcRect.height),
    };

    SDL_Rect dest{
        static_cast<int32_t>(destRect.x + this->xOffset),
        static_cast<int32_t>(destRect.y + this->yOffset),
        static_cast<int32_t>(destRect.width),
        static_cast<int32_t>(destRect.height),
    };

    SetTextureTintColor(texturePtr, tintColor);

    SDL_RenderCopy(this->renderer, texturePtr, &src, &dest);
}

void SDLRenderer::ClearScreen(const int64_t color) {
    SetRenderDrawColor(color);
    SDL_RenderClear(this->renderer);
}

bool SDLRenderer::SetRenderTarget(std::shared_ptr<Texture> renderTarget) {
    if (SDL_SetRenderTarget(this->renderer, ToRawTexture(this->renderTarget)) != 0) {
        LOG_ERROR(SDL_GetError());
        return false;
    }

    this->renderTarget = renderTarget;
    this->xOffset = this->yOffset = 0;
    this->clipRectStack.clear();
    this->offsetStack.clear();
    SDL_RenderSetClipRect(this->renderer, nullptr);
    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
    this->drawColor = 0xFFFFFFFF;

    return true;
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

void SDLRenderer::SetRenderDrawColor(const int64_t color) noexcept {
    if (this->drawColor != color) {
        // TODO: consider opacity
        if (IsBigEndian()) {
            SDL_SetRenderDrawColor(
                this->renderer,
                (color & 0xFF00) >> 8,
                (color & 0xFF0000) >> 16,
                (color & 0xFF000000) >> 24,
                color > COLOR32 ? static_cast<uint8_t>(color & 0xFF) : 255);
        } else {
            SDL_SetRenderDrawColor(
                this->renderer,
                (color & 0xFF0000) >> 16,
                (color & 0xFF00) >> 8,
                color & 0xFF,
                color > COLOR32 ? static_cast<uint8_t>((color & 0xFF000000) >> 24) : 255);
        }

        this->drawColor = color;
    }
}

void SDLRenderer::Attach(SDL_Window* window) {
    auto driverIndex{ 0 };

    this->renderer = SDL_CreateRenderer(window, driverIndex, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!this->renderer) {
        throw std::runtime_error(Format("Failed to create an SDL renderer. SDL Error: %s", SDL_GetError()));
    }

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

    this->renderTarget = nullptr;

    SDL_DestroyRenderer(this->renderer);

    this->renderer = nullptr;
    this->width = 0;
    this->height = 0;
}

void SDLRenderer::Destroy() {
    this->Detach();
}

static void SetTextureTintColor(SDL_Texture* texture, const int64_t color) noexcept {
    // TODO: consider opacity
    if (IsBigEndian()) {
        SDL_SetTextureColorMod(texture, (color & 0xFF00) >> 8, (color & 0xFF0000) >> 16, (color & 0xFF000000) >> 24);
        SDL_SetTextureAlphaMod(texture, color > COLOR32 ? static_cast<uint8_t>(color & 0xFF) : 255);
    } else {
        SDL_SetTextureColorMod(texture, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF);
        SDL_SetTextureAlphaMod(texture, color > COLOR32 ? static_cast<uint8_t>((color & 0xFF000000) >> 24) : 255);
    }
}

static SDL_Texture* ToRawTexture(const std::shared_ptr<Texture>& texture) noexcept {
    if (texture) {
        return static_cast<SDLTexture*>(texture.get())->ToRawTexture();
    } else {
        return nullptr;
    }
}

} // namespace ls
