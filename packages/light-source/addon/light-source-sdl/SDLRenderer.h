/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/Renderer.h>
#include <SDL.h>
#include <vector>

namespace ls {

class SDLTexture;

class SDLRenderer : public Renderer {
 public:
    SDLRenderer();
    virtual ~SDLRenderer();

    int32_t GetWidth() const override { return this->width; }
    int32_t GetHeight() const override { return this->height; }
    PixelFormat GetTextureFormat() const override { return this->textureFormat; }

    bool SetRenderTarget(std::shared_ptr<Texture> renderTarget) override;
    void Present() override;
    void SetClipRect(const Rect& rect) override;
    void ClearClipRect() override;

    void DrawFillRect(const Rect& rect, const uint32_t fillColor) override;
    void DrawBorder(const Rect& rect, const EdgeRect& border, const uint32_t borderColor) override;
    void DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const uint32_t tintColor) override;
    void DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const EdgeRect& capInsets,
        const uint32_t tintColor) override;
    void DrawQuad(const std::shared_ptr<Texture>& texture, const Rect& srcRect, const Rect& destRect,
        const uint32_t tintColor) override;
    void FillRenderTarget(const uint32_t color) override;

    std::shared_ptr<Texture> CreateRenderTarget(const int32_t width, const int32_t height) override;
    std::shared_ptr<Texture> CreateTextureFromSurface(const Surface& surface) override;
    std::shared_ptr<Texture> CreateTexture(const int32_t width, const int32_t height) override;

    void Attach(SDL_Window* window);
    void Detach();
    void Destroy();

    Uint32 GetRawTextureFormat() const noexcept { return this->sdlTextureFormat; }
    SDL_Renderer* ToRawRenderer() noexcept { return this->renderer; }

 private:
    void SetRenderDrawColor(const uint32_t color) noexcept;
    void UpdateTextureFormats(const SDL_RendererInfo& info) noexcept;
    std::shared_ptr<Texture> CreateFillRectTexture() noexcept;

 private:
    SDL_Renderer* renderer{};
    Uint32 sdlTextureFormat{SDL_PIXELFORMAT_UNKNOWN};
    PixelFormat textureFormat{PixelFormatUnknown};
    uint32_t drawColor{};
    std::shared_ptr<Texture> renderTarget;
    std::shared_ptr<Texture> fillRectTexture;
    int32_t width{0};
    int32_t height{0};
};

} // namespace ls
