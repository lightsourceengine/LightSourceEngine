/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "StyleEnums.h"
#include "Resource.h"
#include "Task.h"
#include <memory>

struct stbtt_fontinfo;

namespace ls {

class Font;
class FontStore;
class Stage;

class FontResource : public Resource<std::string> {
 public:
    FontResource(const std::string& family, StyleFontStyle style, StyleFontWeight weight,
                 const std::string& uri, int32_t index) noexcept;
    virtual ~FontResource() noexcept;

    const std::string& GetFontFamily() const noexcept { return this->family; }
    StyleFontStyle GetFontStyle() const noexcept { return this->style; }
    StyleFontWeight GetFontWeight() const noexcept { return this->weight; }
    int32_t GetIndex() const noexcept { return this->index; }
    const std::string& GetUri() const noexcept { return this->uri; }
    std::shared_ptr<Font> GetFont() const { return this->font; }
    bool IsSame(const std::string& family, StyleFontStyle style, StyleFontWeight weight) const noexcept;

 private:
    void Load(Stage* stage);
    void Reset();
    void CtxUninstall();

 private:
    std::string family;
    StyleFontStyle style;
    StyleFontWeight weight;
    std::string uri;
    int32_t index;
    Task fontLoadingTask;
    std::shared_ptr<Font> font;
    bool ctxInstalled{false};

    friend FontStore;
};

} // namespace ls
