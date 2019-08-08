/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Renderer.h>
#include <SDL.h>
#include <vector>
#include <unordered_map>

namespace ls {

class SDLRenderer : public Renderer {
 public:
    SDLRenderer();
    virtual ~SDLRenderer();

    int32_t GetWidth() const override;

    int32_t GetHeight() const override;

    void Reset() override;

    void Present() override;

    void Shift(float x, float y) override;

    void Unshift() override;

    void PushClipRect(const Rect& rect) override;

    void PopClipRect() override;

    void DrawFillRect(const Rect& rect, const int64_t fillColor) override;

    void DrawBorder(const Rect& rect, const EdgeRect& border, const int64_t borderColor) override;

    void DrawImage(const uint32_t textureId, const Rect& rect, const int64_t tintColor) override;

    void DrawImage(
        const uint32_t textureId, const Rect& rect, const EdgeRect& capInsets, const uint32_t tintColor) override;

    void DrawQuad(
        const uint32_t textureId, const Rect& srcRect, const Rect& destRect, const int64_t tintColor) override;

    uint32_t CreateTexture(
        const uint8_t* source, PixelFormat sourceFormat, const int32_t width, const int32_t height) override;

    void DestroyTexture(const uint32_t textureId) override;

    PixelFormat GetTextureFormat() const override { return this->textureFormat; }

    void Attach(SDL_Window* window);

    void Detach();

    void Destroy();

 private:
    void SetRenderDrawColor(const int64_t color);
    void UpdateTextureFormats(const SDL_RendererInfo& info);

 private:
    static uint32_t nextTextureId;

    SDL_Renderer* renderer{};
    Uint32 sdlTextureFormat{SDL_PIXELFORMAT_UNKNOWN};
    PixelFormat textureFormat{PixelFormatUnknown};
    float xOffset{};
    float yOffset{};
    std::vector<SDL_Rect> clipRectStack{};
    std::vector<float> offsetStack{};
    std::unordered_map<uint32_t, SDL_Texture*> textures{};
    int64_t drawColor{};
};

} // namespace ls
