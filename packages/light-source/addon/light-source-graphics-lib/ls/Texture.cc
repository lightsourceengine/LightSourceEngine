/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/Texture.h>

namespace ls {

static Texture::Bridge CreateNullTextureBridge() noexcept;
static Texture::Bridge sNullTextureBridge = CreateNullTextureBridge();

Texture::Texture() noexcept : nativeTexture(nullptr), bridge(&sNullTextureBridge) {
}

Texture::Texture(void* nativeTexture, Bridge* bridge) noexcept : nativeTexture(nativeTexture), bridge(bridge) {
}

int32_t Texture::Width() const noexcept {
    return this->bridge->GetWidth(this->nativeTexture);
}

int32_t Texture::Height() const noexcept {
    return this->bridge->GetHeight(this->nativeTexture);
}

bool Texture::Update(const uint8_t* pixels, int32_t pitch) const noexcept {
    return this->bridge->Update(this->nativeTexture, pixels, pitch);
}

Texture::Pixels Texture::Lock() noexcept {
    void* data{};
    int32_t pitch{};

    if (this->bridge->Lock(this->nativeTexture, &data, &pitch)) {
        return Pixels(this, static_cast<uint8_t*>(data), pitch);
    }

    return {};
}

bool Texture::IsRenderTarget() const noexcept {
    return this->bridge->GetType(this->nativeTexture) == RenderTarget;
}

bool Texture::IsLockable() const noexcept {
    return this->bridge->GetType(this->nativeTexture) == Lockable;
}

bool Texture::IsUpdatable() const noexcept {
    return this->bridge->GetType(this->nativeTexture) == Updatable;
}

PixelFormat Texture::PixelFormat() const noexcept {
    return this->bridge->GetPixelFormat(this->nativeTexture);
}

void Texture::Destroy() noexcept {
    this->bridge->Destroy(this->nativeTexture); this->nativeTexture = nullptr;
}

bool Texture::Empty() const noexcept {
    return this->nativeTexture == nullptr;
}

Texture::operator bool() const noexcept {
    return this->nativeTexture != nullptr;
}

Texture::Pixels::Pixels(Texture* lockedTexture, uint8_t* data, int32_t pitch) noexcept
: lockedTexture(lockedTexture), data(data), pitch(pitch) {
}

Texture::Pixels::~Pixels() noexcept {
    this->Release();
}

uint8_t* Texture::Pixels::Data() const noexcept {
    return this->data;
}

int32_t Texture::Pixels::Pitch() const noexcept {
    return this->pitch;
}

void Texture::Pixels::Release() noexcept {
    if (this->lockedTexture) {
        if (this->lockedTexture->bridge) {
            this->lockedTexture->bridge->Unlock(this->lockedTexture->nativeTexture);
        }
        this->lockedTexture = nullptr;
        this->data = nullptr;
        this->pitch = 0;
    }
}

static Texture::Bridge CreateNullTextureBridge() noexcept {
    return {
        // GetWidth()
        [](void* p) -> int32_t {
            return 0;
        },
        // GetHeight()
        [](void* p) -> int32_t {
            return 0;
        },
        // Lock()
        [](void* p, void** data, int32_t* pitch) -> bool {
            return false;
        },
        // Unlock
        [](void* p) {
        },
        // Update()
        [](void* p, const uint8_t* pixels, int32_t pitch) -> bool {
            return false;
        },
        // GetPixelFormat()
        [](void* p) -> PixelFormat {
            return PixelFormatUnknown;
        },
        // GetType()
        [](void* p) -> Texture::Type {
            return Texture::Type::Updatable;
        },
        // Destroy()
        [](void* p) {
        }
    };
}

} // namespace ls
