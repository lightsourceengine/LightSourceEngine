/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <SafeObjectWrap.h>

namespace ls {

class Renderer;

/**
 * Provides an interface for a Scene to communicate with the native window pr screen object.
 */
class SceneAdapter : public virtual Napi::SafeObjectWrapBase {
 public:
    virtual void Attach(const Napi::CallbackInfo& info) = 0;
    virtual void Detach(const Napi::CallbackInfo& info) = 0;
    virtual void Resize(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetTitle(const Napi::CallbackInfo& info) = 0;
    virtual void SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value) = 0;
    virtual Napi::Value GetWidth(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetHeight(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetFullscreen(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetDisplayIndex(const Napi::CallbackInfo& info) = 0;

    /**
     * Get the size of the native window.
     */
    virtual int32_t GetWidth() const = 0;

    /**
     * Get the height of the native window.
     */
    virtual int32_t GetHeight() const = 0;

    /**
     * Get the renderer interface for drawing.
     *
     * The renderer should only be accessed while the scene is attached to the native window. Also, a Detach() call
     * may invalidate this pointer.
     */
    virtual Renderer* GetRenderer() const = 0;
};

} // namespace ls
