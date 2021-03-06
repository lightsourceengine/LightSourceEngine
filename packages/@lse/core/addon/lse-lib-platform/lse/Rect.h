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

#include <cstdint>

namespace lse {

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

struct IntRect {
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

/**
 * Rectangle with texture coordinates (src member).
 */
struct ImageRect {
  Rect dest{};
  IntRect src{};
};

/**
 * Checks if a rectangle is empty (width or height <= 0).
 */
inline bool IsEmpty(const Rect& rect) noexcept {
  return (static_cast<int32_t>(rect.width) <= 0) || (static_cast<int32_t>(rect.height) <= 0);
}

/**
 * Checks if am ImageRect is empty (dest width or height <= 0).
 */
inline bool IsEmpty(const ImageRect& rect) noexcept {
  return IsEmpty(rect.dest);
}

/**
 * Get the intersection of two rectangles.
 *
 * If the rectangles do not intersect, an empty rectangle is returned.
 */
Rect Intersect(const Rect& a, const Rect& b) noexcept;

/**
 * Clip an arbitrarily sized image (and it's texture coordinates) to a region of the screen.
 *
 * @param bounds The region of the screen the image will be rendered and clipped to. Size is in screen pixels.
 * @param imageDest The region of the screen to render the image to. Size is in screen pixels, but width and height
 * may or may not match the images width and height.
 * @param imageWidth The actual width of the image in pixels.
 * @param imageHeight The actual height of the image in pixels.
 * @return struct containing clipped destination rect and clipped source rect (texture coordinates)
 */
ImageRect ClipImage(const Rect& bounds, const Rect& imageDest, float imageWidth, float imageHeight) noexcept;

/**
 * Move a rectangle by an x and y delta.
 *
 * @param rect The rectangle to move.
 * @param tx x delta
 * @param ty y delta
 * @return Copy of parameter rect with x and y translated.
 */
inline Rect Translate(const Rect& rect, float tx, float ty) noexcept {
  return {
      rect.x + tx,
      rect.y + ty,
      rect.width,
      rect.height
  };
}

} // namespace lse
