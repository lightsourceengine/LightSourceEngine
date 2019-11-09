/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/Rect.h>
#include <string>
#include <napi.h>

namespace ls {

class ImageUri {
 public:
    ImageUri() noexcept = default;
    ImageUri(const std::string& uri) noexcept; // NOLINT
    ImageUri(const std::string& uri, const std::string& id, const int32_t width, const int32_t height) noexcept;
    ImageUri(const std::string& uri, const std::string& id, const int32_t width, const int32_t height,
        const EdgeRect& capInsets) noexcept;

    const std::string& GetId() const noexcept { return this->id.empty() ? this->uri : this->id; }
    const std::string& GetUri() const noexcept { return this->uri; }
    int32_t GetWidth() const noexcept { return this->width; }
    int32_t GetHeight() const noexcept { return this->height; }
    const EdgeRect& GetCapInsets() const noexcept { return this->capInsets; }
    bool HasCapInsets() const noexcept { return this->hasCapInsets; }

    Napi::Value ToObject(const Napi::Env& env) const;
    static ImageUri FromObject(const Napi::Object& spec);

    bool operator==(const ImageUri& rhs) const noexcept { return this->GetId() == rhs.GetId(); }

 private:
    std::string uri;
    std::string id;
    int32_t width{};
    int32_t height{};
    EdgeRect capInsets{};
    bool hasCapInsets{false};
};

} // namespace ls
