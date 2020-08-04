/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "Resources.h"
#include "DecodeImage.h"
#include "DecodeFont.h"
#include <ls/Log.h>

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

void ImageData::Load(Napi::Env env) {
    this->work.Reset(
        env,
        [id = this->id]() {
            return DecodeImageFromFile(id, { 0, 0 });
        },
        [this](Napi::Env env, AsyncWorkResult<BLImage>* result) {
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

Napi::Value ImageData::GetSummary(const Napi::Env& env) const {
    auto summary{ Napi::Object::New(env) };

    if (!this->resource.empty()) {
        summary.Set("width", Napi::Number::New(env, this->resource.width()));
        summary.Set("height", Napi::Number::New(env, this->resource.height()));
    }

    return summary;
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

bool Resources::HasImageData(const std::string& path) const {
    return this->images.find(path) != this->images.end();
}

ImageData* Resources::AcquireImageData(const std::string& path) {
    auto it{ this->images.find(path) };

    if (it != this->images.end()) {
        it->second.refs++;

        return it->second.ToPointer();
    }

    auto result{ this->images.emplace(path, ImageDataRef(std::make_unique<ImageData>(path))) };

    return result.first->second.ToPointer();
}

void Resources::ReleaseImageData(const std::string& path, bool immediateDelete) {
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
        this->ReleaseImageData(resource->id, immediateDelete);
    }
}

void Resources::Compact() {
}

} // namespace ls
