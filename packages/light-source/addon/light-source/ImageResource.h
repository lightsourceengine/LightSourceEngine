/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Renderer.h>
#include "Resource.h"
#include "napi-ext.h"

namespace ls {

class ResourceManager;

class ImageUri {
 public:
    ImageUri() = default;
    explicit ImageUri(const std::string& uri) : uri(uri) {}
    ImageUri(const std::string& uri, const std::string& id, int32_t width, int32_t height)
            : uri(uri), id(id), width(width), height(height) {}
    ImageUri(const std::string& uri, const std::string& id, int32_t width, int32_t height, const EdgeRect& capInsets)
        : uri(uri), id(id), width(width), height(height), capInsets(capInsets),
          hasCapInsets(capInsets.top || capInsets.right || capInsets.bottom || capInsets.left) {}

    const std::string GetId() const { return this->id.empty() ? this->uri : this->id; }
    const std::string GetUri() const { return this->uri; }
    int32_t GetWidth() const { return this->width; }
    int32_t GetHeight() const { return this->height; }
    const EdgeRect& GetCapInsets() const { return this->capInsets; }
    bool HasCapInsets() const { return this->hasCapInsets; }

    bool operator==(const ImageUri& rhs) { return this->GetId() == rhs.GetId(); }

    Napi::Value ToObject(Napi::Env env);
    static ImageUri FromObject(const Napi::Object& spec);

 private:
    std::string uri;
    std::string id;
    int32_t width{};
    int32_t height{};
    EdgeRect capInsets{};
    bool hasCapInsets{false};
};

struct ImageInfo {
    int32_t width;
    int32_t height;
    PixelFormat format;
    std::shared_ptr<uint8_t> data;
};

class ImageResource : public Resource {
 public:
    explicit ImageResource(Napi::Env env, const ImageUri& uri);
    virtual ~ImageResource() = default;

    uint32_t GetTextureId() { return this->textureId; }
    int32_t GetWidth() const { return this->width; }
    int32_t GetHeight() const { return this->height; }
    bool HasCapInsets() const { return this->uri.HasCapInsets(); }
    const EdgeRect& GetCapInsets() const { return this->uri.GetCapInsets(); }

    bool Sync(Renderer* renderer);

 private:
    void Load(Renderer* renderer,
        const std::vector<std::string>& extensions, const std::vector<std::string>& resourcePath);
    uint32_t UpdateTexture(Renderer* renderer);

 private:
    ImageUri uri;
    int32_t width;
    int32_t height;
    std::unique_ptr<Napi::AsyncTask<ImageInfo>> task;
    std::shared_ptr<ImageInfo> image;
    uint32_t textureId{};

    friend ResourceManager;
};

} // namespace ls
