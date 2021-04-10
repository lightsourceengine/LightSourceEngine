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
#include <std20/bit>

#include "Color.h"
#include "PixelFormat.h"

namespace lse {

/**
 * In place conversion of a buffer of RGBA color values to the specified pixel format.
 *
 * The format of the pixel buffer must be PixelFormatRGBA (BE) or PixelFormatABGR (LE). If another format, the
 * behavior is undefined.
 *
 * @param pixels Buffer containing a list of 4 bytes pixels.
 * @param len Number of pixels in the buffer.
 * @param format Pixel format to convert to
 */
void ConvertToFormat(color_t* pixels, int32_t len, PixelFormat format) noexcept;

} // namespace lse
