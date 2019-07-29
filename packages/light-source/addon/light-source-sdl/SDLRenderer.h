/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Renderer.h>
#include <SDL.h>

namespace ls {

class SDLRenderer : public Renderer {
 public:
    ~SDLRenderer() = default;

    void Reset() override {}

    void Shift(int32_t x, int32_t y) override {}
    void Unshift() override {}

    void PushClipRect(const Rect& rect) override {}
    void PopClipRect() override {}

    void DrawFillRect(const Rect& rect, const uint32_t fillColor) override {}

    void DrawBorder(const Rect& rect, const EdgeRect& border, const uint32_t borderColor) override {}

    void DrawImage(const uint32_t textureId, const Rect& rect, const uint32_t tintColor) override {}

    void DrawImage(
        const uint32_t textureId,
        const Rect& rect,
        const EdgeRect& capInsets,
        const uint32_t tintColor) override {}

    uint32_t AddTexture(
        const uint8_t* source,
        PixelFormat sourceFormat,
        const int32_t width,
        const int32_t height) override { return 0; }

    void RemoveTexture(const uint32_t textureId) override {}

 private:
    SDL_Renderer* renderer;
};

} // namespace ls
