/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "StyleUtils.h"
#include <algorithm>
#include <ls/Format.h>

namespace ls {

std::string CreateRoundedRectangleUri(const int32_t radiusTopLeft, const int32_t radiusTopRight,
                                      const int32_t radiusBottomRight, const int32_t radiusBottomLeft,
                                      const int32_t stroke) {
    const std::string empty{};
    static const char* uriTemplate{
        "data:image/svg+xml,<svg viewBox=\"0 0 %i %i\">"
        "<path d=\"M %i,0 h%i %i v%i %i h-%i %i v-%i %i z\" "
        "fill=\"%s\" "
        "stroke=\"%s\" "
        "stroke-width=\"%i\"/>"
        "</svg>"
    };

    const auto leftWidth{ std::max(radiusTopLeft, radiusBottomLeft) };
    const auto rightWidth{ std::max(radiusTopRight, radiusBottomRight) };
    const auto width{ leftWidth + rightWidth + 1 };

    const auto topHeight{ std::max(radiusTopLeft, radiusTopRight) };
    const auto bottomHeight{ std::max(radiusBottomLeft, radiusBottomRight) };
    const auto height{ topHeight + bottomHeight + 1 };

    return Format(uriTemplate,
        /* 0: viewbox       */ width,
        /* 1: viewbox       */ height,
        /* 2: M {},0        */ radiusTopLeft,
        /* 3: h             */ (radiusTopLeft == 0 ? leftWidth : 0) + 1 + (radiusTopRight == 0 ? rightWidth : 0),
        /* 4: a             */ radiusTopRight > 0 ?
            Format("a%i,%i 0 0 1 %i,%i", radiusTopRight, radiusTopRight, radiusTopRight, radiusTopRight) : empty,
        /* 5: v             */ (radiusTopRight == 0 ? topHeight : 0) + 1 + (radiusBottomRight == 0 ? bottomHeight : 0),
        /* 6: a             */ radiusBottomRight > 0 ?
            Format(" a%i,%i 0 0 1 -%i,%i", radiusBottomRight, radiusBottomRight, radiusBottomRight, radiusBottomRight)
                                                     : empty,
        /* 7: h-            */ (radiusBottomLeft == 0 ? leftWidth : 0) + 1 + (radiusBottomRight == 0 ? rightWidth : 0),
        /* 8: a             */ radiusBottomLeft > 0 ?
            Format("a%i,%i 0 0 1 -%i,-%i", radiusBottomLeft, radiusBottomLeft, radiusBottomLeft, radiusBottomLeft)
            : empty,
        /* 9: v-            */ (radiusTopLeft == 0 ? topHeight : 0) + 1 + (radiusBottomLeft == 0 ? bottomHeight : 0),
        /* 10: a            */ radiusTopLeft > 0 ?
            Format("a%i,%i 0 0 1 %i,-%i", radiusTopLeft, radiusTopLeft, radiusTopLeft, radiusTopLeft) : empty,
        /* 11: fill         */ stroke > 0 ? "none" : "white",
        /* 12: stroke       */ stroke > 0 ? "white" : "none",
        /* 13: stroke-width */ stroke);
}

} // namespace ls
