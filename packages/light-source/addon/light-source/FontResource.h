/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "StyleEnums.h"
#include "Resource.h"
#include "Task.h"
#include "FontId.h"
#include <memory>
#include <unordered_map>

struct stbtt_fontinfo;

namespace ls {

class Font;
class FontStore;
class Stage;

class FontResource : public Resource<FontId> {
 public:
    FontResource(const FontId& fontId, const std::string& uri, const int32_t index) noexcept;
    virtual ~FontResource() noexcept;

    const std::string& GetFontFamily() const noexcept { return this->id.family; }
    StyleFontStyle GetFontStyle() const noexcept { return this->id.style; }
    StyleFontWeight GetFontWeight() const noexcept { return this->id.weight; }
    int32_t GetIndex() const noexcept { return this->index; }
    const std::string& GetUri() const noexcept { return this->uri; }
    std::shared_ptr<Font> GetFont(int32_t fontSize) const;

 private:
    void Load(Stage* stage);
    void Reset();

 private:
    std::string uri;
    int32_t index{0};
    mutable std::shared_ptr<stbtt_fontinfo> fontInfo;
    mutable std::unordered_map<int32_t, std::shared_ptr<Font>> fontsBySize;
    Task fontLoadingTask;

    friend FontStore;
};

} // namespace ls
