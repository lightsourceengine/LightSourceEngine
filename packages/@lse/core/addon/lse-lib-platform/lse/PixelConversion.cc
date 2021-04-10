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
