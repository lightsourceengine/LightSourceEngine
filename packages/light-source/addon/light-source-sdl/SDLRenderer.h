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
    ~SDLRenderer() = default;

    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    void Reset() override;
    void Present() override;

    void Shift(float x, float y) override;
    void Unshift() override;

    void PushClipRect(const Rect& rect) override;
    void PopClipRect() override;

    void DrawFillRect(const Rect& rect, const uint32_t fillColor) override;

    void DrawBorder(const Rect& rect, const EdgeRect& border, const uint32_t borderColor) override;

    void DrawImage(const uint32_t textureId, const Rect& rect, const uint32_t tintColor) override;

    void DrawImage(
        const uint32_t textureId,
        const Rect& rect,
        const EdgeRect& capInsets,
        const uint32_t tintColor) override;

    uint32_t AddTexture(
        const uint8_t* source,
        PixelFormat sourceFormat,
        const int32_t width,
        const int32_t height) override;

    void RemoveTexture(const uint32_t textureId) override;

    void Attach(SDL_Window* window);
    void Detach();

 private:
    void SetRenderDrawColor(uint32_t color);

 private:
    static uint32_t nextTextureId;

    SDL_Renderer* renderer{};
    Uint32 textureFormat{SDL_PIXELFORMAT_UNKNOWN};
    float xOffset{};
    float yOffset{};
    std::vector<SDL_Rect> clipRectStack{};
    std::vector<float> offsetStack{};
    std::unordered_map<uint32_t, SDL_Texture*> textures{};
    uint32_t drawColor{};
};

} // namespace ls
