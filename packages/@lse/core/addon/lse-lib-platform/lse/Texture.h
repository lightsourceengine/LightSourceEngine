/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <lse/PixelFormat.h>
#include <memory>

namespace lse {

class Renderer;

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

 public:
  Texture(std::shared_ptr<Renderer> owner,
          void* texture,
          int32_t width,
          int32_t height,
          PixelFormat format,
          Type type) noexcept;
  virtual ~Texture() = default;

  void Destroy() noexcept;

  int32_t Width() const noexcept;
  int32_t Height() const noexcept;
  int32_t Pitch() const noexcept;
  PixelFormat Format() const noexcept;
  bool IsRenderTarget() const noexcept;
  bool IsLockable() const noexcept;
  bool IsUpdatable() const noexcept;

  template<typename T>
  T* As() const noexcept { return static_cast<T*>(this->platformTexture); }

  virtual bool Update(const uint8_t* pixels) noexcept = 0;
  virtual uint8_t* Lock() noexcept = 0;
  virtual void Unlock() noexcept = 0;

  static Texture* SafeDestroy(Texture* texture) noexcept;

 protected:
  std::shared_ptr<Renderer> owner{};
  void* platformTexture{};
  int32_t width{};
  int32_t height{};
  PixelFormat format{};
  Type type{};
};

class TextureLock {
 public:
  TextureLock(Texture* texture);
  ~TextureLock();

  bool IsLocked() const noexcept;
  uint8_t* GetPixels() const noexcept;

 private:
  Texture* texture{};
  uint8_t* pixels{};
};

} // namespace lse
