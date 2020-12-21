/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <memory>

#include "PixelFormat.h"

namespace lse {

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
    int32_t Width() const noexcept;
    int32_t Height() const noexcept;
    int32_t Pitch() const noexcept;

    /**
     * Invalidate pixel access. After this call, Data() will return nullptr.
     */
    void Release() noexcept;

   private:
    Pixels(Texture* lockedTexture, uint8_t* data, int32_t width, int32_t height, int32_t pitch) noexcept;

   private:
    Texture* lockedTexture{};
    uint8_t* data{};
    int32_t width{};
    int32_t height{};
    int32_t pitch{};

    friend class Texture;
  };

  /**
   * Interface between Texture and the native rendering platform.
   */
  class Bridge {
   public:
    virtual ~Bridge() = default;

    virtual int32_t GetWidth(void* platformTextureRef) const noexcept = 0;
    virtual int32_t GetHeight(void* platformTextureRef) const noexcept = 0;
    virtual bool Lock(void* platformTextureRef, void** buffer, int32_t* pitch) noexcept = 0;
    virtual void Unlock(void* platformTextureRef) noexcept = 0;
    virtual bool Update(void* platformTextureRef, const uint8_t* buffer, int32_t length) = 0;
    virtual PixelFormat GetPixelFormat(void* platformTextureRef) const noexcept = 0;
    virtual Type GetType(void* platformTextureRef) const noexcept = 0;
    virtual void Destroy(void* platformTextureRef) noexcept = 0;
  };

 public:
  Texture() noexcept;
  Texture(void* platformTextureRef, Bridge* bridge) noexcept;

  int32_t Width() const noexcept;
  int32_t Height() const noexcept;
  PixelFormat Format() const noexcept;
  bool IsRenderTarget() const noexcept;
  bool IsLockable() const noexcept;
  bool IsUpdatable() const noexcept;

  bool Update(const uint8_t* pixels, int32_t pitch) const noexcept;
  Pixels Lock() noexcept;
  template<typename T>
  T* Cast() const noexcept { return static_cast<T*>(this->platformTextureRef); }

  bool Empty() const noexcept;
  operator bool() const noexcept;

  void Destroy() noexcept;

 private:
  void* platformTextureRef{};
  Bridge* bridge{};
};

} // namespace lse
