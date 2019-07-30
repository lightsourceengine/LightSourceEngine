/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Renderer.h>
#include "Resource.h"
#include "napi-ext.h"

struct NSVGimage;

namespace ls {

class ResourceManager;

class ImageResource : public Resource {
 public:
    explicit ImageResource(Napi::Env env, const std::string& id);
    virtual ~ImageResource() = default;

    uint32_t GetTexture(Renderer* renderer);
    int32_t GetWidth() const { return this->width; }
    int32_t GetHeight() const { return this->height; }
    bool HasCapInsets() const { return false; }
    const EdgeRect& GetCapInsets() const { return this->capInsets; }

 protected:
    void Load(Renderer* renderer,
        const std::vector<std::string>& extensions, const std::vector<std::string>& resourcePath);

 private:
    void LoadImage(const std::string& uriOrFilename,
        const std::vector<std::string>& extensions, const std::vector<std::string>& resourcePath);
    void LoadImageFromSvg(NSVGimage* svgImage, const int32_t scaleWidth, const int32_t scaleHeight);

 private:
    std::string uri;
    std::unique_ptr<AsyncWork> work;
    int32_t width;
    int32_t height;
    std::shared_ptr<uint8_t> data;
    PixelFormat format;
    EdgeRect capInsets;

    friend ResourceManager;
};

} // namespace ls
