/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <cassert>
#include <YGNode.h>
#include <lse/Rect.h>
#include <lse/Point.h>

namespace lse {

bool YGNodeHasDimensions(YGNodeConstRef node) noexcept;

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
