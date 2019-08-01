/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "StyleEnums.h"
#include "napi-ext.h"
#include "Resource.h"
#include <memory>
#include <stb_truetype.h>

namespace ls {

class ResourceManager;

struct FontInfo {
    stbtt_fontinfo stbFontInfo{};
    std::unique_ptr<uint8_t[]> ttf;
};

class FontResource : public Resource {
 public:
    explicit FontResource(Napi::Env env, const std::string& id, const std::string& uri, const int32_t index,
        const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);
    virtual ~FontResource() = default;

    static std::string MakeId(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);
    const std::string& GetFamily() const { return this->family; }
    StyleFontStyle GetFontStyle() const { return this->fontStyle; }
    StyleFontWeight GetFontWeight() const { return this->fontWeight; }

    stbtt_fontinfo* GetFontInfo() { return this->fontInfo ? &this->fontInfo->stbFontInfo : nullptr; }

 private:
    std::string uri;
    int32_t index;
    std::string family;
    StyleFontStyle fontStyle{};
    StyleFontWeight fontWeight{};
    std::shared_ptr<FontInfo> fontInfo{};
    std::unique_ptr<AsyncWork<FontInfo>> work;

 private:
    void Load(const std::vector<std::string>& path);

    friend ResourceManager;
};

} // namespace ls
