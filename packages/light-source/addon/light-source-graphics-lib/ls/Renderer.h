/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/Rect.h>
#include <ls/Texture.h>
#include <ls/Matrix.h>
#include <ls/Color.h>
#include <ls/PixelFormat.h>
#include <cstdint>

namespace ls {

/**
 * Interface for rendering to the screen and creating textures (images).
 */
class Renderer {
 public:
    virtual ~Renderer() = default;

    /**
     * Update the screen with any rendering performed since the previous call.
     *
     * If vsync is enabled, this method will block waiting for the next vertical blank.
     */
    virtual void Present() = 0;

    /**
     * Resets the state of the renderer.
     *
     * Clipping is disabled. The render target is reset to the screen.
     */
    virtual void Reset() = 0;

    /**
     * @return The width of the renderable area in pixels.
     */
    virtual int32_t GetWidth() const = 0;

    /**
     * @return The height of renderable area in pixels.
     */
    virtual int32_t GetHeight() const = 0;

    /**
     * Sets the target texture of the renderer.
     *
     * To reset the render target to the screen (default behavior), use Reset().
     *
     * @param renderTarget A texture of Type::RenderTarget.
     * @return true if the render target was successfully set, false if renderTarget is incompatible as a render target
     */
    virtual bool SetRenderTarget(const Texture& renderTarget) = 0;

    /**
     * Clip draw calls by a rectangular region.
     */
    virtual void EnabledClipping(const Rect& rect) = 0;

    /**
     * Turn off clipping.
     */
    virtual void DisableClipping() = 0;

    /**
     * Draw a solid rectangle.
     */
    virtual void DrawFillRect(const Rect& rect, const Matrix& transform, color_t fillColor) = 0;

    /**
     * Draw a hollow rectangle.
     */
    virtual void DrawBorder(const Rect& rect, const EdgeRect& border, const Matrix& transform, color_t fillColor) = 0;

    /**
     * Draw a textured rectangle.
     */
    virtual void DrawImage(const Texture& texture, const Rect& rect, const Matrix& transform, color_t tintColor) = 0;

    /**
     * Draw a textured rectangle stretched with end cap insets.
     *
     * If the image is sliced into 9 equal parts, the four corners, or end caps, are drawn at a fixed size according
     * to the capInsets arg. The remaining parts are rendered stretched to fill the remaining space. This is typically
     * used for UI buttons or message box backgrounds.
     */
    virtual void DrawImage(const Texture& texture, const EdgeRect& capInsets, const Rect& rect,
            const Matrix& transform, color_t tintColor) = 0;
    /**
     * Clear the entire renderable area with the specified color.
     */
    virtual void FillRenderTarget(color_t color) = 0;

    /**
     * Create a new texture.
     *
     * @param width Width, in pixels, of new texture.
     * @param height Height, in pixels, of new texture.
     * @param type The type of texture to create.
     * @return on success, a valid texture; on failure, an empty texture
     */
    virtual Texture CreateTexture(int32_t width, int32_t height, Texture::Type type) = 0;

    /**
     * Get the pixel format for all new textures.
     */
    virtual PixelFormat GetTextureFormat() const = 0;
};

} // namespace ls
