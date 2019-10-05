/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "yoga-ext.h"
#include <cmath>

static bool FloatsEqual(const float a, const float b) noexcept {
    return std::fabs(a - b) < 0.0001f;
}

float YGRoundValueToPixelGrid(const float value) noexcept {
    const float fractial{ ::fmodf(value, 1.0f) };

    if (std::isnan(fractial)) {
        return value;
    }

    float roundedValue;

    if (FloatsEqual(fractial, 0)) {
        roundedValue = value - fractial;
    } else if (FloatsEqual(fractial, 1.0f)) {
        roundedValue = value - fractial + 1.0f;
    } else {
        roundedValue = value - fractial + (fractial >= 0.5f ? 1.0f : 0.0f);
    }

    return std::isnan(roundedValue) ? YGUndefined : roundedValue;
}
