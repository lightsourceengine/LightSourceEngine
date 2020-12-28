/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/Resources.h>

#include <cassert>
#include <lse/DecodeImage.h>
#include <lse/DecodeFont.h>
#include <lse/Log.h>
#include <lse/Renderer.h>
#include <lse/Math.h>
#include <lse/Uri.h>

namespace lse {

Resource::ResourceId Resource::nextResourceId{ 1 };
constexpr const std::size_t kMaxFonts = 9;

Resource::Resource(const std::string& tag) : id(nextResourceId++), tag(tag) {
  const auto t{ GetUriScheme(tag) };

  if (t == UriSchemeFile) {
    this->path = GetPathFromFileUri(tag);
  }

  if (this->path.empty()) {
    this->path = tag;
  }
}

Napi::String Resource::GetErrorMessage(const Napi::Env& env) const {
  return Napi::String::New(env, this->errorMessage);
}

void Resource::NotifyListeners() {
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

void Resource::AddListener(Owner owner, Listener&& listener) {
  assert(owner != nullptr);
  assert(listener != nullptr);

  if (owner == nullptr || listener == nullptr) {
    return;
  }

  const auto hasOwner{
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

void Resource::RemoveListener(Owner owner) {
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
  const auto t{ GetUriScheme(tag) };
  int32_t resizeWidth{};
  int32_t resizeHeight{};

  if (t == UriSchemeFile) {
    resizeWidth = GetQueryParamInteger(this->tag, "width", 0);
    resizeHeight = GetQueryParamInteger(this->tag, "height", 0);
  }

  this->work.Reset(
      env,
      [path = this->path, resizeWidth, resizeHeight]() {
        return DecodeImageFromFile(path, resizeWidth, resizeHeight);
      },
      [this](Napi::Env env, AsyncWorkResult<ImageBytes>* result) {
        constexpr auto LAMBDA_FUNCTION = "ResourceLoadComplete";

        if (this->state != Resource::State::Loading) {
          return;
        }

        if (result->HasError()) {
          this->state = Resource::State::Error;
          this->errorMessage = result->TakeError();
          this->width = 0;
          this->height = 0;
          LOG_ERROR_LAMBDA("%s", this->errorMessage);
        } else {
          this->state = Resource::State::Ready;
          this->resource = result->TakeValue();
          this->width = this->resource.Width();
          this->height = this->resource.Height();
          LOG_INFO_LAMBDA("%s", this->tag);
        }

        this->NotifyListeners();
      });

  this->state = Resource::State::Loading;
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
  if (this->GetState() != Resource::Ready) {
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

  // TODO: Move pixel conversion to a better place. image loading thread?
  this->resource.SyncFormat(this->texture.Format());

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

Image Image::Mock(const std::string& tag, int32_t width, int32_t height) {
  auto image{ Image(tag) };

  image.width = width;
  image.height = height;

  return image;
}

FontFace::FontFace(const std::string& tag) : Resource(tag) {
  const auto t{ GetUriScheme(tag) };

  if (t == UriSchemeFile) {
    this->family = GetQueryParam(tag, "family");
    this->index = GetQueryParamInteger(tag, "index", 0);

    auto value{ GetQueryParam(tag, "style") };

    if (!value.empty()) {
      try {
        this->style = FromString<StyleFontStyle>(value.c_str());
      } catch (const std::invalid_argument& e) {
        LOG_WARN("Invalid font style parameter: %s", value);
      }
    }

    value = GetQueryParam(tag, "weight");

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

bool FontFace::Equals(
    const FontFaceRef& fontFace, const std::string& family, StyleFontStyle style,
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

        if (this->state != Resource::State::Loading) {
          return;
        }

        if (result->HasError()) {
          this->state = Resource::State::Error;
          this->errorMessage = result->TakeError();
          LOG_ERROR_LAMBDA("%s", this->errorMessage);
        } else {
          this->state = Resource::State::Ready;
          this->resource = result->TakeValue();
          LOG_INFO_LAMBDA("%s", this->tag);
        }

        this->NotifyListeners();
      });

  this->state = Resource::State::Loading;
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
    if (lse::Equals(fontSize, p->blFont.size())) {
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

    if (this->fontsBySize.size() > kMaxFonts) {
      this->fontsBySize.pop_back();
    }
  } else {
    LOG_WARN("Failed to load font %s for size %f", this->family, fontSize);
  }

  return font;
}

bool Resources::HasImage(const std::string& tag) const {
  return this->images.contains(tag);
}

ImageRef Resources::AcquireImage(const std::string& tag) {
  if (tag.empty()) {
    return nullptr;
  }

  if (this->images.contains(tag)) {
    return this->images[tag];
  }

  auto resource{ std::make_shared<Image>(tag) };

  this->images[tag] = resource;

  return resource;
}

bool Resources::HasFontFace(const std::string& tag) const {
  return this->fonts.contains(tag);
}

FontFaceRef Resources::AcquireFontFace(const std::string& tag) {
  if (tag.empty()) {
    return nullptr;
  }

  if (this->fonts.contains(tag)) {
    return this->fonts[tag];
  }

  auto resource{ std::make_shared<FontFace>(tag) };

  this->fonts[tag] = resource;

  return resource;
}

FontFaceRef Resources::AcquireFontFaceByStyle(const std::string& family, StyleFontStyle style, StyleFontWeight weight) {
  for (auto& entry : this->fonts) {
    if (FontFace::Equals(entry.second, family, style, weight)) {
      return entry.second;
    }
  }

  // TODO: if exact match fails, find a substitute
  return nullptr;
}

void Resources::ReleaseResource(Resource* resource, bool immediateDelete) {
  if (!resource) {
    return;
  }

  if (HasFontFace(resource->tag)) {
    if (this->fonts[resource->tag].use_count() == 1) {
      if (immediateDelete) {
        this->pendingDeletions.erase(resource);
        this->fonts.erase(resource->tag);
      } else {
        this->pendingDeletions.insert(resource);
      }
    }
  } else if (HasImage(resource->tag)) {
    if (this->images[resource->tag].use_count() == 1) {
      if (immediateDelete) {
        this->pendingDeletions.erase(resource);
        this->images.erase(resource->tag);
      } else {
        this->pendingDeletions.insert(resource);
      }
    }
  }
}

void Resources::Compact() {
  if (this->pendingDeletions.empty()) {
    return;
  }

  for (auto resource : this->pendingDeletions) {
    if (this->images.contains(resource->tag)) {
      if (this->images[resource->tag].use_count() == 1) {
        this->images.erase(resource->tag);
      }
    } else if (this->fonts.contains(resource->tag)) {
      if (this->fonts[resource->tag].use_count() == 1) {
        this->fonts.erase(resource->tag);
      }
    }
  }

  this->pendingDeletions.clear();
}

} // namespace lse
