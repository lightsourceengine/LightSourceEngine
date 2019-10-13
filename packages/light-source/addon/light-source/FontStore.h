/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "FontResource.h"
#include "ResourceStore.h"

namespace ls {

class Stage;

class FontStore : public ResourceStore<FontResource, FontId, FontId::Hash> {
 public:
    FontStore() noexcept;

    void Attach(Stage* stage) noexcept;
    void Detach() noexcept;
    bool IsAttached() const noexcept { return this->stage != nullptr; }

    void AddFont(const FontId& fontId, const std::string& uri, const int32_t index);
    void RemoveFont(const FontId& fontId);
    std::shared_ptr<FontResource> FindFont(const std::string& family, const StyleFontStyle style,
        const StyleFontWeight weight) const;

 private:
    Stage* stage{nullptr};
};

} // namespace ls
