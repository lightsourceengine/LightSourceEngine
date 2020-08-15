/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/yoga-ext.h>

namespace ls {

Rect YGNodeGetBox(YGNodeConstRef node) noexcept {
    assert(node != nullptr);
    const auto& layout{ node->getLayout() };

    return {
        layout.position[YGEdgeLeft],
        layout.position[YGEdgeTop],
        layout.dimensions[YGDimensionWidth],
        layout.dimensions[YGDimensionHeight],
    };
}

Rect YGNodeGetBox(YGNodeConstRef node, float x, float y) noexcept {
    assert(node != nullptr);
    const auto& layout{ node->getLayout() };

    return {
        x,
        y,
        layout.dimensions[YGDimensionWidth],
        layout.dimensions[YGDimensionHeight],
    };
}

Point YGNodeGetBoxPosition(YGNodeConstRef node) noexcept {
    assert(node != nullptr);
    const auto& layout{ node->getLayout() };

    return {
        layout.position[YGEdgeLeft],
        layout.position[YGEdgeTop],
    };
}

Rect YGNodeGetBorderBox(YGNodeConstRef node) noexcept {
    assert(node != nullptr);
    const auto& layout{ node->getLayout() };

    return {
        layout.border[YGEdgeLeft],
        layout.border[YGEdgeTop],
        layout.dimensions[YGDimensionWidth] - layout.border[YGEdgeLeft] - layout.border[YGEdgeRight],
        layout.dimensions[YGDimensionHeight] - layout.border[YGEdgeTop] - layout.border[YGEdgeBottom],
    };
}

Rect YGNodeGetPaddingBox(YGNodeConstRef node) noexcept {
    assert(node != nullptr);
    const auto& layout{ node->getLayout() };

    return {
        layout.border[YGEdgeLeft] + layout.padding[YGEdgeLeft],
        layout.border[YGEdgeTop] + layout.padding[YGEdgeTop],
        layout.dimensions[YGDimensionWidth] - layout.border[YGEdgeLeft] - layout.border[YGEdgeRight]
            - layout.padding[YGEdgeLeft] - layout.padding[YGEdgeRight],
        layout.dimensions[YGDimensionHeight] - layout.border[YGEdgeTop] - layout.border[YGEdgeBottom]
            - layout.padding[YGEdgeTop] - layout.padding[YGEdgeBottom],
    };
}

EdgeRect YGNodeGetBorderEdges(YGNodeConstRef node) noexcept {
    assert(node != nullptr);
    const auto& layout{ node->getLayout() };
    const auto& border{ layout.border };

    return {
        static_cast<int32_t>(border[YGEdgeTop]),
        static_cast<int32_t>(border[YGEdgeRight]),
        static_cast<int32_t>(border[YGEdgeBottom]),
        static_cast<int32_t>(border[YGEdgeLeft]),
    };
}

const std::vector<YGNodeRef>& YGNodeGetChildren(YGNodeConstRef node) noexcept {
    assert(node != nullptr);
    return node->getChildren();
}

} // namespace ls
