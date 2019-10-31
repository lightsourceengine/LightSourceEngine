/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <tuple>
#include <YGNode.h>
#include <ls/Rect.h>

namespace ls {

/**
 * Get the node's computed rectangle.
 *
 * The position of the rectangle is relative to the parent node.
 */
Rect YGNodeLayoutGetRect(YGNodeConstRef node) noexcept;

/**
 * Get the node's computed rectangle with a custom position.
 */
Rect YGNodeLayoutGetRect(YGNodeConstRef node, float x, float y) noexcept;

/**
 * Get the node's computed border edges.
 */
EdgeRect YGNodeLayoutGetBorderRect(YGNodeConstRef node) noexcept;

/**
 * Get the node's innermost rectangle that takes border and padding into account.
 *
 * The position of the rectangle is relative to (0, 0).
 */
Rect YGNodeLayoutGetInnerRect(YGNodeConstRef node) noexcept;

} // namespace ls
