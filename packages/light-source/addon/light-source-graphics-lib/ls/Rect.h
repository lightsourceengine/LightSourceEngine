/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>

namespace ls {

/**
 * Rectangle in TRBL positional value format.
 */
struct EdgeRect {
    int32_t top;
    int32_t right;
    int32_t bottom;
    int32_t left;
};

/**
 * Rectangle in position (x, y) and size (width, height) format.
 */
struct Rect {
    float x;
    float y;
    float width;
    float height;
};

/**
 * Checks if a rectangle is empty (width or height <= 0).
 */
inline bool IsEmpty(const Rect& rect) noexcept {
    return (static_cast<int32_t>(rect.width) <= 0) || (static_cast<int32_t>(rect.height) <= 0);
}

/**
 * Get the intersection of two rectangles.
 *
 * If the rectangles do not intersect, an empty rectangle is returned.
 */
inline Rect Intersect(const Rect& a, const Rect& b) noexcept {
    const auto x{ (b.x > a.x) ? b.x : a.x };
    const auto y{ (b.y > a.y) ? b.y : a.y };
    const auto ax2{ a.x + a.width };
    const auto bx2{ b.x + b.width };
    const auto ay2{ a.y + a.height };
    const auto by2{ b.y + b.height };

    return {
        x,
        y,
        (bx2 < ax2) ? bx2 - x : ax2 - x,
        (by2 < ay2) ? by2 - y : ay2 - y,
    };
}

} // namespace ls
