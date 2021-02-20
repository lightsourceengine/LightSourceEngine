/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <lse/AsyncWork.h>
#include <lse/StyleEnums.h>
#include <lse/Texture.h>
#include <lse/ImageBytes.h>
#include <lse/types.h>

#include <memory>
#include <string>
#include <list>
#include <phmap.h>
#include <std17/filesystem>

namespace lse {

class Renderer;

class Resource {
 public:
  using Owner = void*;
  using Listener = std::function<void(Owner, Resource*)>;
  using ResourceId = uint32_t;

  enum State {
    Init,
    Ready,
    Error,
    Loading,
  };

 public:
  Resource(const std::string& tag);
  virtual ~Resource() = default;

  ResourceId GetId() const noexcept { return this->id; }
  const std::string& GetTag() const noexcept { return this->tag; }

  void AddListener(Owner owner, Listener&& listener);
  void RemoveListener(Owner owner);
  State GetState() const noexcept { return this->state; }
  virtual void Load(Napi::Env env) = 0;

  const std::string& GetErrorMessage() const noexcept;
  virtual Napi::Value Summarize(const Napi::Env& env) const = 0;

 protected:
  void NotifyListeners();

 protected:
  struct ListenerEntry {
    Owner owner{};
    Listener listener;
  };

  static ResourceId nextResourceId;

  ResourceId id;
  std::string tag{};
  std17::filesystem::path path{};
  std::vector<ListenerEntry> listeners{};
  State state{ Init };
  std::string errorMessage{};

  friend class Resources;
};

class Image final : public Resource {
 public:
  Image(const std::string& tag) : Resource(tag) {}
  ~Image() override = default;

  void Load(Napi::Env env) override;
  Napi::Value Summarize(const Napi::Env& env) const override;

  bool LoadTexture(Renderer* renderer);
  bool HasTexture() const noexcept;
  Texture* GetTexture() const noexcept;

  bool HasDimensions() const noexcept;
  int32_t Width() const noexcept;
  int32_t Height() const noexcept;
  float WidthF() const noexcept;
  float HeightF() const noexcept;
  float AspectRatio() const noexcept;

  // Test-only method.
  static Image Mock(const std::string& tag, int32_t width, int32_t height);

 private:
  AsyncWork<ImageBytes> work;
  ImageBytes resource{};
  Texture* texture{};
  int32_t width{};
  int32_t height{};
};

class Resources {
 public:
  bool HasImage(const std::string& tag) const;
  ImageRef AcquireImage(const std::string& tag);

  void ReleaseResource(Resource* resource, bool immediateDelete = false);

  void Compact();

 private:
  using ImageDataMap = phmap::flat_hash_map<std::string, ImageRef>;

  ImageDataMap images;
  phmap::flat_hash_set<Resource*> pendingDeletions;
};

} // namespace lse
