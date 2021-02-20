/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/Resources.h>

#include <cassert>
#include <lse/DecodeImage.h>
#include <lse/Log.h>
#include <lse/Renderer.h>
#include <lse/Uri.h>

namespace lse {

Resource::ResourceId Resource::nextResourceId{ 1 };

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

  if (this->texture) {
    this->texture = Texture::SafeDestroy(this->texture);
  }

  this->texture = renderer->CreateTexture(
      this->resource.Width(), this->resource.Height(), Texture::Type::Updatable);

  if (!this->texture) {
    return false;
  }

  // TODO: Move pixel conversion to a better place. image loading thread?
  this->resource.SyncFormat(this->texture->Format());

  // TODO: keep image pixels?
  return this->texture->Update(this->resource.Bytes());
}

bool Image::HasTexture() const noexcept {
  return static_cast<bool>(this->texture);
}

Texture* Image::GetTexture() const noexcept {
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



void Resources::ReleaseResource(Resource* resource, bool immediateDelete) {
  if (!resource) {
    return;
  }

  if (HasImage(resource->tag)) {
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
    }
  }

  this->pendingDeletions.clear();
}

} // namespace lse
