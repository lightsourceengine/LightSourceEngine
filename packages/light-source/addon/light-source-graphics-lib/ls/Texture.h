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

/**
 * GPU texture.
 */
class Texture {
 public:
    enum Type {
        /** Texture can be Update()'d with pixel data. */
        Updatable,
        /** Texture can be Lock()'d to access pixel data for writing. */
        Lockable,
        /** Texture can be used as a render target in Renderer. Render targets can be Lock()'d and Update()'d. */
        RenderTarget,
    };

    /**
     * Returned by Lock() to allow pixel write access.
     *
     * When this object goes out of scope, the destructor will call Release().
     */
    class Pixels {
     public:
        Pixels() noexcept = default;
        ~Pixels() noexcept;

        uint8_t* Data() const noexcept;
        int32_t Pitch() const noexcept;

        /**
         * Invalidate pixel access. After this call, Data() will return nullptr.
         */
        void Release() noexcept;

     private:
        Pixels(Texture* lockedTexture, uint8_t* data, int32_t pitch) noexcept;

     private:
        Texture* lockedTexture{};
        uint8_t* data{};
        int32_t pitch{};

        friend class Texture;
    };

    /**
     * Interface between Texture and the native rendering platform.
     */
    struct Bridge {
        int32_t (*GetWidth)(void*);
        int32_t (*GetHeight)(void*);
        bool (*Lock)(void*, void**, int32_t*);
        void (*Unlock)(void*);
        bool (*Update)(void*, const uint8_t*, int32_t);
        PixelFormat (*GetPixelFormat)(void*);
        Type (*GetType)(void*);
        void (*Destroy)(void*);
    };

 public:
    Texture() noexcept;
    Texture(void* nativeTexture, Bridge* bridge) noexcept;

    int32_t Width() const noexcept;
    int32_t Height() const noexcept;
    PixelFormat Format() const noexcept;
    bool IsRenderTarget() const noexcept;
    bool IsLockable() const noexcept;
    bool IsUpdatable() const noexcept;

    bool Update(const uint8_t* pixels, int32_t pitch) const noexcept;
    Pixels Lock() noexcept;
    template<typename T>
    T* Cast() const noexcept { return static_cast<T*>(this->nativeTexture); }

    bool Empty() const noexcept;
    operator bool() const noexcept;

    void Destroy() noexcept;

 private:
    void* nativeTexture{};
    Bridge* bridge{};
};

} // namespace ls
