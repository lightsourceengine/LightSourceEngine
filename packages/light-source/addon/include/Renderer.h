/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>

namespace ls {

enum PixelFormat {
    PixelFormatRGBA,
    PixelFormatARGB,
    PixelFormatABGR,
    PixelFormatBGRA,
    PixelFormatAlpha,
    PixelFormatUnknown
};

struct EdgeRect {
    int32_t top;
    int32_t right;
    int32_t bottom;
    int32_t left;
};

struct Rect {
    float x;
    float y;
    float width;
    float height;
};

class Renderer {
 public:
    virtual ~Renderer() = default;

    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
    virtual void Reset() = 0;
    virtual void Present() = 0;

    virtual void Shift(float x, float y) = 0;
    virtual void Unshift() = 0;

    virtual void PushClipRect(const Rect& rect) = 0;
    virtual void PopClipRect() = 0;

    virtual void DrawFillRect(const Rect& rect, const uint32_t fillColor) = 0;

    virtual void DrawBorder(const Rect& rect, const EdgeRect& border, const uint32_t borderColor) = 0;

    virtual void DrawImage(const uint32_t textureId, const Rect& rect, const uint32_t tintColor) = 0;

    virtual void DrawImage(
        const uint32_t textureId,
        const Rect& rect,
        const EdgeRect& capInsets,
        const uint32_t tintColor) = 0;

    virtual uint32_t AddTexture(
        const uint8_t* source,
        PixelFormat sourceFormat,
        const int32_t width,
        const int32_t height) = 0;

    virtual void RemoveTexture(const uint32_t textureId) = 0;

    virtual PixelFormat GetTextureFormat() const = 0;
};

} // namespace ls
