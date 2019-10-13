/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontResource.h"
#include <fmt/println.h>
#include <functional>

namespace ls {

FontId::FontId(const std::string& family, const StyleFontStyle style, const StyleFontWeight weight) noexcept
: family(family), style(style), weight(weight) {
}

bool FontId::operator==(const FontId &other) const noexcept {
    return this->family == other.family && this->style == other.style && this->weight == other.weight;
}

FontId::operator std::string() const {
    return fmt::format("{}-{}-{}",
        this->family, StyleFontStyleToString(this->style), StyleFontWeightToString(this->weight));
}

std::size_t FontId::Hash::operator() (const FontId& id) const noexcept {
    return std::hash<std::string>()(id.family)
        ^ static_cast<std::size_t>(id.style)
        ^ static_cast<std::size_t>(id.weight);
}

} // namespace ls
