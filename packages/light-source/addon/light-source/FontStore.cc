/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontStore.h"
#include "Stage.h"

namespace ls {

void FontStore::AddFont(const std::string& family, StyleFontStyle style, StyleFontWeight weight,
                        const std::string& uri, int32_t index) {
    assert(this->IsAttached());

    auto it{ this->families.find(family) };

    if (it != this->families.end()) {
        if (it->second.fonts[style][weight]) {
            throw std::runtime_error("Font has already been added.");
        }
    } else {
        this->families.insert(std::make_pair(family, FontSet()));
        it = this->families.find(family);
    }

    auto font{ std::make_shared<FontResource>(family, style, weight, uri, index) };

    font->Load(this->stage);

    it->second.fonts[style][weight] = font;
}

void FontStore::RemoveFont(const std::string& family, StyleFontStyle style, StyleFontWeight weight) {
    assert(this->IsAttached());

    auto it{ this->families.find(family) };

    if (it != this->families.end()) {
        it->second.fonts[style][weight].reset();
    }
}

std::shared_ptr<FontResource> FontStore::FindFont(const std::string& family, const StyleFontStyle style,
        const StyleFontWeight weight) const {
    assert(this->IsAttached());

    auto it{ this->families.find(family) };

    if (it == this->families.end()) {
        return nullptr;
    }

    std::shared_ptr<FontResource> p = it->second.fonts[style][weight];

    if (p) {
        return p;
    }

    for (int32_t s = 0; s < Count<StyleFontStyle>(); s++) {
        for (int32_t w = 0; w < Count<StyleFontWeight>(); w++) {
            if (s != style && w != weight) {
                p = it->second.fonts[s][w];
                if (p) {
                    return p;
                }
            }
        }
    }

    return nullptr;
}

void FontStore::Attach(Stage* stage) noexcept {
    assert(!this->IsAttached());

    this->stage = stage;
}

void FontStore::Detach() noexcept {
    if (!this->IsAttached()) {
        return;
    }

    this->stage = nullptr;
    this->families.clear();
}

} // namespace ls
