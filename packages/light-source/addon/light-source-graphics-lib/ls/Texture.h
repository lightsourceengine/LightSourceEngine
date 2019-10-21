/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/PixelFormat.h>
#include <cstdint>
#include <memory>

namespace ls {

class Surface;

/**
 * GPU texture interface.
 */
class Texture {
 public:
    virtual ~Texture() noexcept = default;

    /**
     * Is this texture attached to the GPU?
     *
     * If attached, this texture can be used with draw operations on the renderer.
     */
    virtual bool IsAttached() const noexcept = 0;

    /**
     * Width of the texture in pixels. If the texture is not attached, width is 0.
     */
    virtual int32_t GetWidth() const noexcept = 0;

    /**
     * Height of the texture in pixels. If the texture is not attached, height is 0.
     */
    virtual int32_t GetHeight() const noexcept = 0;

    /**
     * Get the pixel format of this texture. If the texture is not attached, PixelFormatUnknown is returned.
     */
    virtual PixelFormat GetFormat() const noexcept = 0;

    /**
     * Can this texture be used as a render target by the renderer?
     */
    virtual bool IsRenderTarget() const noexcept = 0;

    /**
     * Is Update() supported by this texture?
     */
    virtual bool CanUpdate() const noexcept = 0;

    /**
     * Copies a surface directly to this texture.
     *
     * @param surface Must be the exact same dimensions and format as the texture.
     * @return false if copy failed; otherwise, true
     */
    virtual bool Update(const Surface& surface) noexcept = 0;

    /**
     * Is Lock() supported by this texture?
     */
    virtual bool CanLock() const noexcept = 0;

    /**
     * Get write only access to a texture's pixels.
     *
     * Do not use this method to read texture pixel data. The pixel data buffer may or may not contain the latest
     * state of the texture (for performance reasons). This method is only for writing data into a texture.
     *
     * To release access to this texture's pixels, call Surface destructor by letting the return value
     * fall out of scope. Note, the pixels must be unlocked before rendering this texture.
     *
     * @return Writable surface. If texture does not exist, surface will be empty.
     */
    virtual Surface Lock() noexcept = 0;
};

} // namespace ls
