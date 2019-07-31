/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLRenderer.h"
#include <fmt/format.h>

namespace ls {

uint32_t SDLRenderer::nextTextureId{ 0 };
SDL_Rect GetRendererSize(SDL_Renderer* renderer);

int32_t SDLRenderer::GetWidth() const {
    return GetRendererSize(this->renderer).w;
}

int32_t SDLRenderer::GetHeight() const {
    return GetRendererSize(this->renderer).h;
}

void SDLRenderer::Reset() {
    this->xOffset = this->yOffset = 0;
    this->clipRectStack.clear();
    this->offsetStack.clear();
    SDL_RenderSetClipRect(this->renderer, nullptr);
    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
    this->drawColor = 0xFFFFFFFF;
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

void SDLRenderer::DrawFillRect(const Rect& rect, const uint32_t fillColor) {
    SDL_Rect tempRect{
        static_cast<int32_t>(rect.x + this->xOffset),
        static_cast<int32_t>(rect.y + this->yOffset),
        static_cast<int32_t>(rect.width),
        static_cast<int32_t>(rect.height),
    };

    this->SetRenderDrawColor(fillColor);

    SDL_RenderFillRect(this->renderer, &tempRect);
}

void SDLRenderer::DrawBorder(const Rect& rect, const EdgeRect& border, const uint32_t borderColor) {
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

void SDLRenderer::DrawImage(const uint32_t textureId, const Rect& rect, const uint32_t tintColor) {
    auto it{ this->textures.find(textureId) };

    if (it == this->textures.end()) {
        return;
    }

    SDL_Rect tempRect{
        static_cast<int32_t>(rect.x + this->xOffset),
        static_cast<int32_t>(rect.y + this->yOffset),
        static_cast<int32_t>(rect.width),
        static_cast<int32_t>(rect.height),
    };

    // TODO: opacity, tint color endianess, move blend mode
    SDL_SetTextureColorMod(it->second, (tintColor & 0xFF0000) >> 16, (tintColor & 0xFF00) >> 8, tintColor & 0xFF);
    SDL_SetTextureAlphaMod(it->second, 255);

    SDL_RenderCopy(this->renderer, it->second, nullptr, &tempRect);
}

void SDLRenderer::DrawImage(const uint32_t textureId, const Rect& rect, const EdgeRect& capInsets,
        const uint32_t tintColor) {
    auto it{ this->textures.find(textureId) };

    if (it == this->textures.end()) {
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
    auto texture{ it->second };

    // TODO: opacity, tint color endianess, move blend mode
    SDL_SetTextureColorMod(texture, (tintColor & 0xFF0000) >> 16, (tintColor & 0xFF00) >> 8, tintColor & 0xFF);
    SDL_SetTextureAlphaMod(texture, 255);
    SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, &textureHeight);

    // Top row

    srcRect = { 0, 0, capInsets.left, capInsets.top };
    destRect = { x, y, capInsets.left, capInsets.top };

    SDL_RenderCopy(this->renderer, texture, &srcRect, &destRect);

    srcRect = { capInsets.left, 0, textureWidth - capInsets.left - capInsets.right, capInsets.top };
    destRect = { x + capInsets.left, y, width - capInsets.left - capInsets.right, capInsets.top };

    SDL_RenderCopy(this->renderer, texture, &srcRect, &destRect);

    srcRect = { textureWidth - capInsets.right, 0, capInsets.right, capInsets.top };
    destRect = { x + width - capInsets.right, y, capInsets.right, capInsets.top };

    SDL_RenderCopy(this->renderer, texture, &srcRect, &destRect);

    // Middle row

    srcRect = { 0, capInsets.top, capInsets.left, textureHeight - capInsets.top - capInsets.bottom };
    destRect = { x, y + capInsets.top, capInsets.left, height - capInsets.top - capInsets.bottom };

    SDL_RenderCopy(this->renderer, texture, &srcRect, &destRect);

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

    SDL_RenderCopy(this->renderer, texture, &srcRect, &destRect);

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

    SDL_RenderCopy(this->renderer, texture, &srcRect, &destRect);

    // Bottom row

    srcRect = { 0, textureHeight - capInsets.bottom, capInsets.left, capInsets.bottom };
    destRect = { x, y + height - capInsets.bottom, capInsets.left, capInsets.bottom };

    SDL_RenderCopy(this->renderer, texture, &srcRect, &destRect);

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

    SDL_RenderCopy(this->renderer, texture, &srcRect, &destRect);

    srcRect = { textureWidth - capInsets.right, textureHeight - capInsets.bottom, capInsets.right, capInsets.bottom };
    destRect = { x + width - capInsets.right, y + height - capInsets.bottom, capInsets.right, capInsets.bottom };

    SDL_RenderCopy(this->renderer, texture, &srcRect, &destRect);
}

uint32_t SDLRenderer::AddTexture(const uint8_t* source, PixelFormat sourceFormat,
        const int32_t width, const int32_t height) {
    void* pixels;
    int pitch;
    auto texture{
        SDL_CreateTexture(this->renderer, this->textureFormat, SDL_TEXTUREACCESS_STREAMING, width, height)
    };

    if (!texture) {
        return 0;
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) == 0) {
        auto dest{ reinterpret_cast<uint8_t*>(pixels) };

        // TODO: is this right?
        if (pitch != 0) {
            auto stride{ width * 4 };

            for (auto h{ 0 }; h < height; h++) {
                memcpy(&dest[h*pitch], source, stride);
                source += stride;
            }
        } else {
            memcpy(dest, source, width * height * 4);
        }

        SDL_UnlockTexture(texture);

        auto textureId{ nextTextureId++ };

        this->textures[textureId] = texture;

        return textureId;
    }

    SDL_DestroyTexture(texture);

    return 0;
}

void SDLRenderer::RemoveTexture(const uint32_t textureId) {
    auto it{ this->textures.find(textureId) };

    if (it == this->textures.end()) {
        return;
    }

    SDL_DestroyTexture(it->second);

    this->textures.erase(textureId);
}

void SDLRenderer::SetRenderDrawColor(uint32_t color) {
    if (this->drawColor != color) {
        SDL_SetRenderDrawColor(
            this->renderer,
            // TODO: endianess
            (color & 0xFF0000) >> 16,
            (color & 0xFF00) >> 8,
            color & 0xFF,
            // TODO: get opacity from rgb
            255);

        this->drawColor = color;
    }
}

void SDLRenderer::Attach(SDL_Window* window) {
    this->renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!this->renderer) {
        throw std::runtime_error(fmt::format("Failed to create an SDL renderer. SDL Error: {}", SDL_GetError()));
    }
}

void SDLRenderer::Detach() {
    if (!this->renderer) {
        return;
    }

    for (auto& p : this->textures) {
        SDL_DestroyTexture(p.second);
    }

    this->textures.clear();

    SDL_DestroyRenderer(this->renderer);
    this->renderer = nullptr;
}

inline
SDL_Rect GetRendererSize(SDL_Renderer* renderer) {
    SDL_Rect rect{};

    if (renderer) {
        SDL_GetRendererOutputSize(renderer, &rect.w, &rect.h);
    }

    return rect;
}

} // namespace ls
