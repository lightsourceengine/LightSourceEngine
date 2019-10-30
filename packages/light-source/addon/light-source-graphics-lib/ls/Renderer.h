/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/Rect.h>
#include <ls/Surface.h>
#include <ls/Texture.h>
#include <ls/PixelFormat.h>
#include <cstdint>
#include <memory>

namespace ls {

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
     * Get the pixel format for all new textures.
     */
    virtual PixelFormat GetTextureFormat() const = 0;

    /**
     * Sets the target texture of the renderer.
     *
     * If renderTarget = nullptr, the renderer is reset and the screen is restored as the render target.
     */
    virtual bool SetRenderTarget(std::shared_ptr<Texture> renderTarget) = 0;

    /**
     * Finish rendering and copy the result to the screen.
     *
     * If vsync is enabled, this method will block waiting for the next vertical blank.
     */
    virtual void Present() = 0;

    /**
     * Set the clip rect.
     */
    virtual void SetClipRect(const Rect& rect) = 0;

    /**
     * Turn off clipping.
     */
    virtual void ClearClipRect() = 0;

    /**
     * Draw a solid color rectangle.
     */
    virtual void DrawFillRect(const Rect& rect, const uint32_t fillColor) = 0;

    /**
     * Draw a rectangle's outline.
     */
    virtual void DrawBorder(const Rect& rect, const EdgeRect& border, const uint32_t borderColor) = 0;

    /**
     * Draw an image.
     */
    virtual void DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const uint32_t tintColor) = 0;

    /**
     * Draw an image with end-cap insets.
     *
     * Each corner of the image is an end-cap, where the size is defined by the edges in the capInsets args. End-caps
     * are rendered 1:1. The areas between the end-caps and the middle are stretched to fill in the remainder of the
     * destination rect area.
     */
    virtual void DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const EdgeRect& capInsets,
        const uint32_t tintColor) = 0;

    /**
     * Draw a portion of an image.
     */
    virtual void DrawQuad(const std::shared_ptr<Texture>& texture, const Rect& srcRect, const Rect& destRect,
        const uint32_t tintColor) = 0;

    /**
     * Clear the entire renderable area with the specified color.
     */
    virtual void FillRenderTarget(const uint32_t color) = 0;

    /**
     * Create a texture that can be used as a render target.
     *
     * Render targets cannot be updated or locked.
     *
     * @param width Width, in pixels, of new texture.
     * @param height Height, in pixels, of new texture.
     * @return texture on success; nullptr if texture could not be created
     */
    virtual std::shared_ptr<Texture> CreateRenderTarget(const int32_t width, const int32_t height) = 0;

    /**
     * Create a new texture from a surface.
     *
     * The surface format must be PixelFormatAlpha or GetTextureFormat().
     *
     * @return texture on success; nullptr if texture could not be created
     */
    virtual std::shared_ptr<Texture> CreateTextureFromSurface(const Surface& surface) = 0;

    /**
     * Create a new texture.
     *
     * @param width Width, in pixels, of new texture.
     * @param height Height, in pixels, of new texture.
     * @return texture on success; nullptr if texture could not be created
     */
    virtual std::shared_ptr<Texture> CreateTexture(const int32_t width, const int32_t height) = 0;
};

} // namespace ls
