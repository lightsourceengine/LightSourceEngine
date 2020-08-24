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
#include <ls/Math.h>
#include <ls/Uri.h>

namespace ls {

constexpr const std::size_t LS_MAX_FONTS = 9;

Res::Res(const std::string& id) : id(id) {
    const auto t{GetUriScheme(id) };

    if (t == UriSchemeFile) {
        this->path = GetPathFromFileUri(id);
    }

    if (this->path.empty()) {
        this->path = id;
    }
}

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
    const auto t{GetUriScheme(id) };
    int32_t resizedWidth{};
    int32_t resizedHeight{};

    if (t == UriSchemeFile) {
        resizedWidth = GetQueryParamInteger(this->id, "width", 0);
        resizedHeight = GetQueryParamInteger(this->id, "height", 0);
    }

    this->work.Reset(
        env,
        [path = this->path, resizedWidth, resizedHeight]() {
            return DecodeImageFromFile(path, resizedWidth, resizedHeight);
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

Napi::Value Image::Summarize(const Napi::Env& env) const {
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

    this->texture = renderer->CreateTexture(this->resource.Width(), this->resource.Height(),
            Texture::Type::Updatable);

    if (!this->texture) {
        return false;
    }

    // TODO: convert to texture format?
    // TODO: keep image pixels?

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

float Image::AspectRatio() const noexcept {
    return this->height > 0 ? this->WidthF() / this->HeightF() : 0;
}

Image Image::Mock(const std::string& id, int32_t width, int32_t height) {
    auto image{ Image(id) };

    image.width = width;
    image.height = height;

    return image;
}

FontFace::FontFace(const std::string& id) : Res(id) {
    const auto t{ GetUriScheme(id) };

    if (t == UriSchemeFile) {
        this->family = GetQueryParam(id, "family");
        this->index = GetQueryParamInteger(id, "index", 0);

        auto value{ GetQueryParam(id, "style") };

        if (!value.empty()) {
            try {
                this->style = FromString<StyleFontStyle>(value.c_str());
            } catch (const std::invalid_argument& e) {
                LOG_WARN("Invalid font style parameter: %s", value);
            }
        }

        value = GetQueryParam(id, "weight");

        if (!value.empty()) {
            try {
                this->weight = FromString<StyleFontWeight>(value.c_str());
            } catch (const std::invalid_argument& e) {
                LOG_WARN("Invalid font weight parameter: %s", value);
            }
        }
    }

    if (this->family.empty()) {
        this->family = this->path.stem();
    }
}

bool FontFace::Equals(FontFace* fontFace, const std::string& family, StyleFontStyle style,
        StyleFontWeight weight) noexcept {
    return fontFace && fontFace->family == family && fontFace->style == style && fontFace->weight == weight;
}

void FontFace::Load(Napi::Env env) {
    this->work.Reset(
        env,
        [path = this->path, index = this->index]() {
          return DecodeFontFromFile(path, index);
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

Napi::Value FontFace::Summarize(const Napi::Env& env) const {
    auto summary{ Napi::Object::New(env) };

    if (!this->resource.empty() && this->resource.familyNameSize() > 0) {
        summary.Set("family", Napi::String::New(env, this->family));
        summary.Set("style", Napi::String::New(env, ToString(this->style)));
        summary.Set("weight", Napi::String::New(env, ToString(this->weight)));
    }

    return summary;
}

const std::string& FontFace::GetFamily() const {
    return this->family;
}

StyleFontStyle FontFace::GetStyle() const noexcept {
    return this->style;
}

StyleFontWeight FontFace::GetWeight() const noexcept {
    return this->weight;
}

Font FontFace::GetFont(float fontSize) {
    if (std::isnan(fontSize) || fontSize <= 0.f) {
        return {};
    }

    for (auto p = this->fontsBySize.begin(); p != this->fontsBySize.end(); p++) {
        if (ls::Equals(fontSize, p->blFont.size())) {
            Font font = *p;

            this->fontsBySize.erase(p);
            this->fontsBySize.push_back(font);

            return font;
        }
    }

    Font font{};

    if (font.blFont.createFromFace(this->resource, fontSize) == BL_SUCCESS) {
        BLTextMetrics ellipsisTextMetrics{};

        this->ellipsis.setUtf8Text("...");
        font.blFont.shape(this->ellipsis);
        font.blFont.getTextMetrics(this->ellipsis, ellipsisTextMetrics);
        font._ellipsisWidth = static_cast<float>(ellipsisTextMetrics.advance.x);

        this->fontsBySize.push_front(font);

        if (this->fontsBySize.size() > LS_MAX_FONTS) {
            this->fontsBySize.pop_back();
        }
    } else {
        LOG_WARN("Failed to load font %s for size %f", this->family, fontSize);
    }

    return font;
}

bool Resources::HasImage(const std::string& path) const {
    return !path.empty() && this->images.find(path) != this->images.end();
}

Image* Resources::AcquireImage(const std::string& path) {
    if (path.empty()) {
        return nullptr;
    }

    auto it{ this->images.find(path) };

    if (it != this->images.end()) {
        it->second.refs++;

        return it->second.ToPointer();
    }

    auto result{ this->images.emplace(path, ImageDataRef(std::make_unique<Image>(path))) };

    return result.first->second.ToPointer();
}

void Resources::ReleaseImage(const std::string& path, bool immediateDelete) {
    if (path.empty()) {
        return;
    }

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
    if (path.empty()) {
        return nullptr;
    }

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
        if (FontFace::Equals(entry.second.ToPointer(), family, style, weight)) {
            entry.second.refs++;
            return entry.second.ToPointer();
        }
    }

    // TODO: if exact match fails, find a substitute
    return nullptr;
}

void Resources::ReleaseFontFace(const std::string& path, bool immediateDelete) {
    if (path.empty()) {
        return;
    }

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
