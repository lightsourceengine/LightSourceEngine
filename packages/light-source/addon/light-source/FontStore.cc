/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontStore.h"
#include "Stage.h"

namespace ls {

FontStore::FontStore() noexcept : ResourceStore<FontResource, FontResource::idType, FontResource::idType::Hash>() {
}

void FontStore::AddFont(const FontId& fontId, const std::string& uri, const int32_t index) {
    assert(this->IsAttached());

    auto font{ std::make_shared<FontResource>(fontId, uri, index) };

    font->Load(this->stage);
    this->Add(font);
}

void FontStore::RemoveFont(const FontId& fontId) {
    assert(this->IsAttached());

    auto font{ this->Get(fontId) };

    if (font) {
        this->Remove(fontId);
        font->Reset();
    }
}

std::shared_ptr<FontResource> FontStore::FindFont(const std::string& family, const StyleFontStyle style,
        const StyleFontWeight weight) const {
    assert(this->IsAttached());

    return this->Get({ family, style, weight });
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
    this->resources.clear();
}

} // namespace ls
