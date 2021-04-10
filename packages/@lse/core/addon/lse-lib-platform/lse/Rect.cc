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

#include "Rect.h"

#include <cmath>
#include <lse/math-ext.h>

namespace lse {

ImageRect ClipImage(const Rect& bounds, const Rect& imageDest, float imageWidth, float imageHeight) noexcept {
  const auto ax2{ bounds.x + bounds.width };
  const auto bx2{ imageDest.x + imageDest.width };
  const auto ay2{ bounds.y + bounds.height };
  const auto by2{ imageDest.y + imageDest.height };
  const auto scaleX{ imageWidth / imageDest.width };
  const auto scaleY{ imageHeight / imageDest.height };
  ImageRect result;

  if (imageDest.x > bounds.x) {
    result.dest.x = imageDest.x;
    result.src.x = 0;
  } else {
    result.dest.x = bounds.x;
    result.src.x = SnapToPixelGrid<int32_t>(std::fabs(bounds.x - imageDest.x) * scaleX);
  }

  if (imageDest.y > bounds.y) {
    result.dest.y = imageDest.y;
    result.src.y = 0;
  } else {
    result.dest.y = bounds.y;
    result.src.y = SnapToPixelGrid<int32_t>(std::fabs(bounds.y - imageDest.y) * scaleY);
  }

  result.dest.width = (bx2 < ax2) ? bx2 - result.dest.x : ax2 - result.dest.x;
  result.dest.height = (by2 < ay2) ? by2 - result.dest.y : ay2 - result.dest.y;

  result.src.width = SnapToPixelGrid<int32_t>(result.dest.width * scaleX);
  result.src.height = SnapToPixelGrid<int32_t>(result.dest.height * scaleY);

  // TODO: snap src rect to pixel grid?

  return result;
}

Rect Intersect(const Rect& a, const Rect& b) noexcept {
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

} // namespace lse
