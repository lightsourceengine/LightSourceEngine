/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace ls {

class Renderer;

struct SceneAdapterConfig {
    int32_t displayIndex;
    int32_t width;
    int32_t height;
    bool fullscreen;
};

/**
 * Provides an interface for a Scene to communicate with the native window pr screen object.
 */
class SceneAdapter {
 public:
    virtual ~SceneAdapter() = default;

    /**
     * Attach the scene to the graphics environment. This enables rendering.
     */
    virtual void Attach() = 0;

    /**
     * Detach the scene from the graphics environment. Graphics resources, such as textures, are released and rendering
     * is disabled.
     */
    virtual void Detach() = 0;

    /**
     * Set the renderable size of the window, in pixels.
     */
    virtual void Resize(int32_t width, int32_t height, bool fullscreen) = 0;

    /**
     * Set the title of the native window.
     */
    virtual void SetTitle(const std::string& title) = 0;

    /**
     * Get the title of the native window.
     */
    virtual std::string GetTitle() const = 0;

    /**
     * Get the size of the native window.
     */
    virtual int32_t GetWidth() const = 0;

    /**
     * Get the height of the native window.
     */
    virtual int32_t GetHeight() const = 0;

    /**
     * Is the native window in fullscreen mode.
     */
    virtual bool GetFullscreen() const = 0;

    /**
     * Get the renderer interface for drawing.
     *
     * The renderer should only be accessed while the scene is attached to the native window. Also, a Detach() call
     * may invalidate this pointer.
     */
    virtual Renderer* GetRenderer() const = 0;
};

} // namespace ls
