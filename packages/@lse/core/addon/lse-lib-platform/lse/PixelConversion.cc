/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "PixelConversion.h"

using std20::endian;

namespace lse {

void ConvertToFormat(color_t* pixels, int32_t len, PixelFormat format) noexcept {
  switch (format) {
    case PixelFormatARGB:
      for (int32_t i = 0; i < len; i++) {
        const color_t temp = pixels[i];

        pixels[i].a = temp.r;
        pixels[i].r = temp.g;
        pixels[i].g = temp.b;
        pixels[i].b = temp.a;
      }
      break;
    case PixelFormatBGRA:
      for (int32_t i = 0; i < len; i++) {
        const color_t temp = pixels[i];

        pixels[i].b = temp.r;
        // pixels[i].g = temp.g;
        pixels[i].r = temp.b;
        // pixels[i].a = temp.a;
      }
      break;
    case PixelFormatABGR:
      for (int32_t i = 0; i < len; i++) {
        const color_t temp = pixels[i];

        pixels[i].a = temp.r;
        pixels[i].b = temp.g;
        pixels[i].g = temp.b;
        pixels[i].r = temp.a;
      }
      break;
    default:
      // PixelFormatRGBA - no op
      break;
  }
}

} // namespace lse
