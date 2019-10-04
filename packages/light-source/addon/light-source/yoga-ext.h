/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Yoga.h>

/**
 * Snaps a float value to the screen's pixel grid.
 *
 * THis is a simplified implementation of Yoga's YGRoundValueToPixelGrid that removes the extra features (floor, ceil
 * and scale factor) that the renderer does not use.
 */
float YGRoundValueToPixelGrid(const float value) noexcept;
