/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/Resources.h>

#include <cassert>
#include <ls/DecodeImage.h>
#include <ls/DecodeFont.h>
#include <ls/Log.h>
#include <ls/Renderer.h>
#include <ls/PixelConversion.h>

namespace ls {

Napi::String Res::GetErrorMessage(const Napi::Env& env) const {
    return Napi::String::New(env, this->errorMessage);
}

void Res::NotifyListeners() {
    for (const auto& entry : this->listeners) {
        if (entry.owner) {
            try {
                entry.listener(entry.owner, this);
            } catch (std::exception& e) {
                LOG_ERROR("%s", e);
            }
        }
    }

    this->listeners.erase(
        std::remove_if(
            this->listeners.begin(),
            this->listeners.end(),
            [](const ListenerEntry& e) { return e.owner == nullptr; }),
        this->listeners.end());
}

void Res::AddListener(Owner owner, Listener&& listener) {
    assert(owner != nullptr);
    assert(listener != nullptr);

    if (owner == nullptr || listener == nullptr) {
        return;
    }

    const auto hasOwner {
        std::find_if(
            this->listeners.cbegin(),
            this->listeners.cend(),
            [owner](const ListenerEntry& e) noexcept { return e.owner == owner; }) != this->listeners.cend()
    };

    assert(!hasOwner);

    if (hasOwner) {
        return;
    }

    this->listeners.emplace_back(ListenerEntry{ owner, listener });
}

void Res::RemoveListener(Owner owner) {
    if (owner == nullptr) {
        return;
    }

    for (auto& entry : this->listeners) {
        if (owner == entry.owner) {
            entry = {};
        }
    }
}

void Image::Load(Napi::Env env) {
    this->work.Reset(
        env,
        [id = this->id]() {
            return DecodeImageFromFile(id, 0, 0);
        },
        [this](Napi::Env env, AsyncWorkResult<ImageBytes>* result) {
            constexpr auto LAMBDA_FUNCTION = "ResourceLoadComplete";

            if (this->state != Res::State::Loading) {
                return;
            }

            if (result->HasError()) {
                this->state = Res::State::Error;
                this->errorMessage = result->TakeError();
                this->width = 0;
                this->height = 0;
                LOG_ERROR_LAMBDA("%s", this->errorMessage);
            } else {
                this->state = Res::State::Ready;
                this->resource = result->TakeValue();
                this->width = this->resource.Width();
                this->height = this->resource.Height();
                LOG_INFO_LAMBDA("%s", this->id);
            }

            this->NotifyListeners();
        });

    this->state = Res::State::Loading;
    this->errorMessage.clear();
    this->resource = {};

    this->work.Queue();
}

Napi::Value Image::GetSummary(const Napi::Env& env) const {
    auto summary{ Napi::Object::New(env) };

    summary.Set("width", Napi::Number::New(env, this->resource.Width()));
    summary.Set("height", Napi::Number::New(env, this->resource.Height()));

    return summary;
}

bool Image::LoadTexture(Renderer* renderer) {
    if (this->GetState() != Ready) {
        return false;
    }

    if (this->HasTexture()) {
        this->texture.Destroy();
    }

    this->texture = renderer->CreateTexture(
            this->resource.Width(), this->resource.Height(), Texture::Type::Updatable);

    if (!this->texture) {
        return false;
    }

// TODO: ConvertToFormat(static_cast<Color*>(imageData.pixelData), imageData.size.w * imageData.size.h,
// renderer->GetTextureFormat());

    return this->texture.Update(this->resource.Bytes(), this->resource.Pitch());
}

bool Image::HasTexture() const noexcept {
    return static_cast<bool>(this->texture);
}

Texture Image::GetTexture() const noexcept {
    return this->texture;
}

bool Image::HasDimensions() const noexcept {
    return this->width > 0 && this->height > 0;
}

int32_t Image::Width() const noexcept {
    return this->width;
}

int32_t Image::Height() const noexcept {
    return this->height;
}

float Image::WidthF() const noexcept {
    return this->width;
}

float Image::HeightF() const noexcept {
    return this->height;
}

bool FontFace::Equals(FontFace* fontFace, const std::string& family,
                   StyleFontStyle style, StyleFontWeight weight) noexcept {
    if (fontFace) {
        return false;
    } else {
        return family.empty();
    }
}

void FontFace::Load(Napi::Env env) {
    this->work.Reset(
        env,
        [id = this->id]() {
          return DecodeFontFromFile(id, 0);
        },
        [this](Napi::Env env, AsyncWorkResult<BLFontFace>* result) {
          constexpr auto LAMBDA_FUNCTION = "ResourceLoadComplete";

          if (this->state != Res::State::Loading) {
              return;
          }

          if (result->HasError()) {
              this->state = Res::State::Error;
              this->errorMessage = result->TakeError();
              LOG_ERROR_LAMBDA("%s", this->errorMessage);
          } else {
              this->state = Res::State::Ready;
              this->resource = result->TakeValue();
              LOG_INFO_LAMBDA("%s", this->id);
          }

          this->NotifyListeners();
        });

    this->state = Res::State::Loading;
    this->errorMessage.clear();
    this->resource.reset();

    this->work.Queue();
}

Napi::Value FontFace::GetSummary(const Napi::Env& env) const {
    auto summary{ Napi::Object::New(env) };

    if (!this->resource.empty()) {
        if (this->resource.familyNameSize() > 0) {
            summary.Set("family", Napi::String::New(env, this->resource.familyName()));
            // TODO: convert to string
            summary.Set("style", Napi::Number::New(env, this->resource.style()));
            summary.Set("weight", Napi::Number::New(env, this->resource.weight()));
        }
    }

    return summary;
}

std::string FontFace::GetFamilyName() const {
    if (this->resource.empty() || this->resource.familyNameSize() == 0) {
        return "";
    } else {
        return this->resource.familyName();
    }
}

bool Resources::HasImage(const std::string& path) const {
    return this->images.find(path) != this->images.end();
}

Image* Resources::AcquireImage(const std::string& path) {
    auto it{ this->images.find(path) };

    if (it != this->images.end()) {
        it->second.refs++;

        return it->second.ToPointer();
    }

    auto result{ this->images.emplace(path, ImageDataRef(std::make_unique<Image>(path))) };

    return result.first->second.ToPointer();
}

void Resources::ReleaseImage(const std::string& path, bool immediateDelete) {
    auto it{ this->images.find(path) };

    if (it != this->images.end()) {
        it->second.refs--;

        if (immediateDelete && it->second.refs <= 0) {
            this->fonts.erase(path);
        }
    }
}

bool Resources::HasFontFace(const std::string& path) const {
    return this->fonts.find(path) != this->fonts.end();
}

FontFace* Resources::AcquireFontFace(const std::string& path) {
    auto it{ this->fonts.find(path) };

    if (it != this->fonts.end()) {
        it->second.refs++;

        return it->second.ToPointer();
    }

    auto result{ this->fonts.emplace(path, FontFaceRef(std::make_unique<FontFace>(path))) };

    return result.first->second.ToPointer();
}

FontFace* Resources::AcquireFontFaceByStyle(const std::string& family, StyleFontStyle style, StyleFontWeight weight) {
    for (auto& entry : this->fonts) {
        if (entry.second.resource->GetFamilyName() == family) {
            entry.second.refs++;
            return entry.second.ToPointer();
        }
    }

    return nullptr;
}

void Resources::ReleaseFontFace(const std::string& path, bool immediateDelete) {
    auto it{ this->fonts.find(path) };

    if (it != this->fonts.end()) {
        it->second.refs--;

        if (immediateDelete && it->second.refs <= 0) {
            this->fonts.erase(path);
        }
    }
}

void Resources::ReleaseResource(Res* resource, bool immediateDelete) {
    if (resource) {
        this->ReleaseFontFace(resource->id, immediateDelete);
        this->ReleaseImage(resource->id, immediateDelete);
    }
}

void Resources::Compact() {
}

} // namespace ls
