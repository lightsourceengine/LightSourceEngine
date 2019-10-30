/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "yoga-ext.h"

namespace ls {

Rect YGNodeLayoutGetRect(YGNodeConstRef node, float x, float y) noexcept {
    const auto& layout{ node->getLayout() };

    return {
        x,
        y,
        layout.dimensions[YGDimensionWidth],
        layout.dimensions[YGDimensionHeight],
    };
}

Rect YGNodeLayoutGetRect(YGNodeConstRef node) noexcept {
    const auto& layout{ node->getLayout() };

    return {
        layout.position[YGEdgeLeft],
        layout.position[YGEdgeTop],
        layout.dimensions[YGDimensionWidth],
        layout.dimensions[YGDimensionHeight],
    };
}

EdgeRect YGNodeLayoutGetBorderRect(YGNodeConstRef node) noexcept {
    const auto& layout{ node->getLayout() };
    const auto& border{ layout.border };

    return {
        static_cast<int32_t >(border[YGEdgeTop]),
        static_cast<int32_t >(border[YGEdgeRight]),
        static_cast<int32_t >(border[YGEdgeBottom]),
        static_cast<int32_t >(border[YGEdgeLeft]),
    };
}

Rect YGNodeLayoutGetInnerRect(YGNodeConstRef node) noexcept {
    const auto& layout{ node->getLayout() };
    const auto& border{ layout.border };
    const auto& padding{ layout.padding };
    const auto& position{ layout.position };
    const auto& dimensions{ layout.dimensions };

    return {
        position[YGEdgeLeft] + border[YGEdgeLeft] + padding[YGEdgeLeft],
        position[YGEdgeRight] + border[YGEdgeRight] + padding[YGEdgeRight],
        dimensions[YGDimensionWidth]
            - border[YGEdgeLeft] - padding[YGEdgeLeft] - border[YGEdgeRight] - padding[YGEdgeRight],
        dimensions[YGDimensionHeight]
            - border[YGEdgeLeft] - padding[YGEdgeLeft] - border[YGEdgeRight] - padding[YGEdgeRight],
    };
}

} // namespace ls
