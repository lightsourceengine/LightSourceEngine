/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>

namespace ls {

/**
 * Pixel formats used for texture creation.
 */
enum PixelFormat {
    /* 4 channel, 32 bit, RGBA */
    PixelFormatRGBA,
    /* 4 channel, 32 bit, ARGB */
    PixelFormatARGB,
    /* 4 channel, 32 bit, ABGR */
    PixelFormatABGR,
    /* 4 channel, 32 bit, BGRA */
    PixelFormatBGRA,
    /* 1 channel, 8 bit, alpha */
    PixelFormatAlpha,
    /* unknown pixel format */
    PixelFormatUnknown
};

/**
 * Rectangle in TRBL positional value format.
 */
struct EdgeRect {
    int32_t top;
    int32_t right;
    int32_t bottom;
    int32_t left;
};

/**
 * Rectangle in position (x, y) and size (width, height) format.
 */
struct Rect {
    float x;
    float y;
    float width;
    float height;
};

/**
 * Interface for rendering to the screen and creating textures (images).
 */
class Renderer {
 public:
    virtual ~Renderer() = default;

    /**
     * Width of renderable area in pixels.
     */
    virtual int32_t GetWidth() const = 0;

    /**
     * Height of renderable area in pixels.
     */
    virtual int32_t GetHeight() const = 0;

    /**
     * Prepares this Renderer for rendering.
     *
     * The registration point is set to 0,0 and the clip rect is cleared.
     */
    virtual void Reset() = 0;

    /**
     * Finish rendering and copy the result to the screen.
     *
     * If vsync is enabled, this method will block waiting for the next vertical blank.
     */
    virtual void Present() = 0;

    /**
     * Move the registration point by dx and dy.
     *
     * The current state of the registration point is pushed onto an internal stack. Then, dx and dy are added to
     * the registration point.
     */
    virtual void Shift(float dx, float dy) = 0;

    /**
     * Restores the registration point to it's previous state.
     *
     * Internally, the renderer keeps a stack of registration points, pushed by Shift(). This method pops a registration
     * point from the stack and sets it as the current registration point.
     */
    virtual void Unshift() = 0;

    /**
     * Set the clipping rectangle for draw calls.
     *
     * The current clip rectangle is pushed onto the stack. The current clip rect is intersected with the new clip
     * rectangle. The intersection is set as the current clip rect.
     */
    virtual void PushClipRect(const Rect& rect) = 0;

    /**
     * Resets the clipping rectangle to it's previous state.
     */
    virtual void PopClipRect() = 0;

    /**
     * Draw a solid color rectangle.
     */
    virtual void DrawFillRect(const Rect& rect, const int64_t fillColor) = 0;

    /**
     * Draw a rectangle's outline.
     */
    virtual void DrawBorder(const Rect& rect, const EdgeRect& border, const int64_t borderColor) = 0;

    /**
     * Draw an image.
     */
    virtual void DrawImage(const uint32_t textureId, const Rect& rect, const int64_t tintColor) = 0;

    /**
     * Draw an image with end-cap insets.
     *
     * Each corner of the image is an end-cap, where the size is defined by the edges in the capInsets args. End-caps
     * are rendered 1:1. The areas between the end-caps and the middle are stretched to fill in the remainder of the
     * destination rect area.
     */
    virtual void DrawImage(
        const uint32_t textureId, const Rect& rect, const EdgeRect& capInsets, const uint32_t tintColor) = 0;

    /**
     * Draw a portion of an image.
     */
    virtual void DrawQuad(
        const uint32_t textureId, const Rect& srcRect, const Rect& destRect, const int64_t tintColor) = 0;

    /**
     * Create a new texture.
     *
     * @param source Image pixels. The length, in bytes, is width * height * (# sourceFormat channels)
     * @param sourceFormat Pixel format of source. This must be PixelFormatAlpha or GetTextureFormat()
     * @param width Width, in pixels, of new texture.
     * @param height Height, in pixels, of new texture.
     * @return 0 - Failed to create a texture; > 0 - Texture created successfully
     */
    virtual uint32_t CreateTexture(
        const uint8_t* source, PixelFormat sourceFormat, const int32_t width, const int32_t height) = 0;

    /**
     * Destroy a texture.
     */
    virtual void DestroyTexture(const uint32_t textureId) = 0;

    /**
     * Get the pixel format for all new textures.
     */
    virtual PixelFormat GetTextureFormat() const = 0;
};

} // namespace ls
