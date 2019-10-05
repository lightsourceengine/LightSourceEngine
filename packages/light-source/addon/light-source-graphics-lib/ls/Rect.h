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

} // namespace ls
