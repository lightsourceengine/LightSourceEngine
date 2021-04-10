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

#include <lse/yoga-ext.h>

#include <lse/math-ext.h>

namespace lse {

bool YGNodeHasDimensions(YGNodeConstRef node) noexcept {
  assert(node != nullptr);
  const auto& layout{ node->getLayout() };

  return static_cast<int32_t>(layout.dimensions[YGDimensionWidth]) > 0
    && static_cast<int32_t>(layout.dimensions[YGDimensionHeight]) > 0;
}

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
      SnapToPixelGrid<int32_t>(border[YGEdgeTop]),
      SnapToPixelGrid<int32_t>(border[YGEdgeRight]),
      SnapToPixelGrid<int32_t>(border[YGEdgeBottom]),
      SnapToPixelGrid<int32_t>(border[YGEdgeLeft]),
  };
}

const std::vector<YGNodeRef>& YGNodeGetChildren(YGNodeConstRef node) noexcept {
  assert(node != nullptr);
  return node->getChildren();
}

} // namespace lse
