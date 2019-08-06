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

struct stbtt_fontinfo;

namespace ls {

class ResourceManager;

class FontResource : public Resource {
 public:
    explicit FontResource(Napi::Env env, const std::string& id, const std::string& uri, const int32_t index,
        const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);
    virtual ~FontResource() = default;

    static std::string MakeId(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);
    const std::string& GetFontFamily() const { return this->family; }
    StyleFontStyle GetFontStyle() const { return this->fontStyle; }
    StyleFontWeight GetFontWeight() const { return this->fontWeight; }

    std::shared_ptr<stbtt_fontinfo> GetFontInfo() { return this->fontInfo; }

 private:
    std::string uri;
    int32_t index;
    std::string family;
    StyleFontStyle fontStyle{};
    StyleFontWeight fontWeight{};
    std::shared_ptr<stbtt_fontinfo> fontInfo{};
    std::unique_ptr<Napi::AsyncTask<stbtt_fontinfo>> task;

 private:
    void Load(const std::vector<std::string>& path);

    friend ResourceManager;
};

} // namespace ls
