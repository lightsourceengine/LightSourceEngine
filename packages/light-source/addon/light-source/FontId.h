/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "StyleEnums.h"
#include <string>

namespace ls {

struct FontId {
    std::string family;
    StyleFontStyle style{};
    StyleFontWeight weight{};

    FontId() noexcept = default;
    FontId(const std::string& family, const StyleFontStyle style, const StyleFontWeight weight) noexcept;

    bool operator==(const FontId &other) const noexcept;

    operator std::string() const;

    struct Hash {
        std::size_t operator() (const FontId& id) const noexcept;
    };
};

} // namespace ls
