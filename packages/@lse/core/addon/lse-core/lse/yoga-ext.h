/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cassert>
#include <YGNode.h>
#include <lse/Rect.h>
#include <lse/Point.h>

namespace lse {

/**
 * Gets the bounding box of an element. The position is relative to the element's parent. The dimensions include
 * the border, padding and content.
 */
Rect YGNodeGetBox(YGNodeConstRef node) noexcept;

/**
 * Gets the bounding box of an element. The position is forced to be set to the passed in x and y coordinates. The
 * dimensions include the border, padding and content.
 */
Rect YGNodeGetBox(YGNodeConstRef node, float x, float y) noexcept;

/**
 *
 */
Point YGNodeGetBoxPosition(YGNodeConstRef node) noexcept;

/**
 * Get the border box of an element. The position is relative to the bounding box. The dimensions include padding
 * and content, which is the area of the bounding box excluding the border.
 */
Rect YGNodeGetBorderBox(YGNodeConstRef node) noexcept;

/**
 * Get the padding box of an element. The position is relative to the bounding box. The dimensions include the content,
 * which is the area of the bounding box excluding border and padding.
 */
Rect YGNodeGetPaddingBox(YGNodeConstRef node) noexcept;

/**
 * @return the top, right, bottom and left values of the element's border.
 */
EdgeRect YGNodeGetBorderEdges(YGNodeConstRef node) noexcept;

/**
 * @return Array of child elements.
 */
const std::vector<YGNodeRef>& YGNodeGetChildren(YGNodeConstRef node) noexcept;

/**
 * @return Element's context object cast to T*.
 */
template<typename T>
T* YGNodeGetContextAs(YGNodeConstRef node) noexcept {
    assert(node != nullptr);
    return static_cast<T*>(node->getContext());
}

} // namespace lse
