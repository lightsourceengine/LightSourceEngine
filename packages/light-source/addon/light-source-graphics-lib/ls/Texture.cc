/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/Texture.h>

namespace ls {

class NullTextureBridge final : public Texture::Bridge {
 public:
    ~NullTextureBridge() override = default;

    int32_t GetWidth(void* platformTextureRef) const noexcept override { return 0; }
    int32_t GetHeight(void* platformTextureRef) const noexcept override { return 0; }
    bool Lock(void* platformTextureRef, void** buffer, int32_t* pitch) noexcept override { return false; }
    void Unlock(void* platformTextureRef) noexcept override {}
    bool Update(void* platformTextureRef, const uint8_t* buffer, int32_t length) override { return false; }
    PixelFormat GetPixelFormat(void* platformTextureRef) const noexcept override { return PixelFormatUnknown; }
    Texture::Type GetType(void* platformTextureRef) const noexcept override { return Texture::Updatable; }
    void Destroy(void* platformTextureRef) noexcept override {}
};

static NullTextureBridge sNullTextureBridge{};

Texture::Texture() noexcept : platformTextureRef(nullptr), bridge(&sNullTextureBridge) {
}

Texture::Texture(void* platformTextureRef, Bridge* bridge) noexcept
: platformTextureRef(platformTextureRef), bridge(bridge) {
}

int32_t Texture::Width() const noexcept {
    return this->bridge->GetWidth(this->platformTextureRef);
}

int32_t Texture::Height() const noexcept {
    return this->bridge->GetHeight(this->platformTextureRef);
}

bool Texture::Update(const uint8_t* pixels, int32_t pitch) const noexcept {
    return this->bridge->Update(this->platformTextureRef, pixels, pitch);
}

Texture::Pixels Texture::Lock() noexcept {
    void* data{};
    int32_t pitch{};

    if (this->bridge->Lock(this->platformTextureRef, &data, &pitch)) {
        return Pixels(this, static_cast<uint8_t*>(data), this->Width(), this->Height(), pitch);
    }

    return {};
}

bool Texture::IsRenderTarget() const noexcept {
    return this->bridge->GetType(this->platformTextureRef) == RenderTarget;
}

bool Texture::IsLockable() const noexcept {
    return this->bridge->GetType(this->platformTextureRef) == Lockable;
}

bool Texture::IsUpdatable() const noexcept {
    return this->bridge->GetType(this->platformTextureRef) == Updatable;
}

PixelFormat Texture::Format() const noexcept {
    return this->bridge->GetPixelFormat(this->platformTextureRef);
}

void Texture::Destroy() noexcept {
    this->bridge->Destroy(this->platformTextureRef); this->platformTextureRef = nullptr;
}

bool Texture::Empty() const noexcept {
    return this->platformTextureRef == nullptr;
}

Texture::operator bool() const noexcept {
    return this->platformTextureRef != nullptr;
}

Texture::Pixels::Pixels(Texture* lockedTexture, uint8_t* data, int32_t width, int32_t height, int32_t pitch) noexcept
: lockedTexture(lockedTexture), data(data), width(width), height(height), pitch(pitch) {
}

Texture::Pixels::~Pixels() noexcept {
    this->Release();
}

uint8_t* Texture::Pixels::Data() const noexcept {
    return this->data;
}

int32_t Texture::Pixels::Width() const noexcept {
    return this->width;
}

int32_t Texture::Pixels::Height() const noexcept {
    return this->height;
}

int32_t Texture::Pixels::Pitch() const noexcept {
    return this->pitch;
}

void Texture::Pixels::Release() noexcept {
    if (this->lockedTexture) {
        if (this->lockedTexture->bridge) {
            this->lockedTexture->bridge->Unlock(this->lockedTexture->platformTextureRef);
        }
        this->lockedTexture = nullptr;
        this->data = nullptr;
        this->pitch = 0;
    }
}

} // namespace ls
