/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "FontResource.h"
#include "ResourceStore.h"
#include <unordered_map>

namespace ls {

class Stage;

class FontStore {
 private:
    struct FontSet {
        std::shared_ptr<FontResource> fonts[Count<StyleFontStyle>()][Count<StyleFontWeight>()]{};
    };

 public:
    void Attach(Stage* stage) noexcept;
    void Detach() noexcept;
    bool IsAttached() const noexcept { return this->stage != nullptr; }

    void AddFont(const std::string& family, StyleFontStyle style, StyleFontWeight weight,
        const std::string& uri, int32_t index);
    void RemoveFont(const std::string& family, StyleFontStyle style, StyleFontWeight weight);
    std::shared_ptr<FontResource> FindFont(const std::string& family,
        StyleFontStyle style, StyleFontWeight weight) const;

    template<typename Callback>
    void ForEach(Callback callback);

 private:
    Stage* stage{nullptr};
    std::unordered_map<std::string, FontSet> families;
};

template<typename Callback>
void FontStore::ForEach(Callback callback) {
    for (auto& p : this->families) {
        for (int32_t style = 0; style < Count<StyleFontStyle>(); style++) {
            for (int32_t weight = 0; weight < Count<StyleFontWeight>(); weight++) {
                if (p.second.fonts[style][weight]) {
                    callback(p.second.fonts[style][weight]);
                }
            }
        }
    }
}

} // namespace ls
