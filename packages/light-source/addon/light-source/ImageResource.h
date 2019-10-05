/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Renderer.h>
#include "Resource.h"
#include "AsyncTaskQueue.h"

namespace ls {

class ResourceManager;

class ImageUri {
 public:
    ImageUri() noexcept = default;
    explicit ImageUri(const std::string& uri) noexcept;
    ImageUri(const std::string& uri, const std::string& id, const int32_t width, const int32_t height) noexcept;
    ImageUri(
        const std::string& uri,
        const std::string& id,
        const int32_t width,
        const int32_t height,
        const EdgeRect& capInsets) noexcept;

    const std::string& GetId() const noexcept { return this->id.empty() ? this->uri : this->id; }
    const std::string& GetUri() const noexcept { return this->uri; }
    int32_t GetWidth() const noexcept { return this->width; }
    int32_t GetHeight() const noexcept { return this->height; }
    const EdgeRect& GetCapInsets() const noexcept { return this->capInsets; }
    bool HasCapInsets() const noexcept { return this->hasCapInsets; }

    bool operator==(const ImageUri& rhs) const noexcept { return this->GetId() == rhs.GetId(); }

    Napi::Value ToObject(const Napi::Env& env) const;
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

    ImageInfo() noexcept = default;
    ImageInfo(
        const int32_t width,
        const int32_t height,
        const PixelFormat format,
        const std::shared_ptr<uint8_t>& data) noexcept;
};

class ImageResource : public Resource {
 public:
    explicit ImageResource(const ImageUri& uri) noexcept;
    virtual ~ImageResource() noexcept;

    uint32_t GetTextureId() const noexcept { return this->textureId; }
    int32_t GetWidth() const noexcept { return this->width; }
    int32_t GetHeight() const noexcept { return this->height; }
    bool HasCapInsets() const noexcept { return this->uri.HasCapInsets(); }
    const EdgeRect& GetCapInsets() const noexcept { return this->uri.GetCapInsets(); }
    const ImageUri& GetUri() const noexcept { return this->uri; }

    bool Sync(Renderer* renderer);

    Napi::Value ToObject(const Napi::Env& env) const;

 private:
    void Load(AsyncTaskQueue* taskQueue, Renderer* renderer, const std::vector<std::string>& extensions,
        const std::vector<std::string>& resourcePath);
    void Detach(Renderer* renderer);
    uint32_t UpdateTexture(Renderer* renderer);

 private:
    ImageUri uri;
    int32_t width;
    int32_t height;
    std::shared_ptr<Task> task;
    std::shared_ptr<ImageInfo> image;
    uint32_t textureId{};

    friend ResourceManager;
};

} // namespace ls
