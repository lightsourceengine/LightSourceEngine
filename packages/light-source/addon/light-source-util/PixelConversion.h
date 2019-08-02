/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Renderer.h>

namespace ls {

/**
 * Endianness of the runtime.
 */
extern const bool isBigEndian;

/**
 * In place conversion of a buffer of RGBA color values to the specified pixel format.
 *
 * @param bytes Byte buffer containing RGBA formatted color values
 * @param len Size of bytes in bytes
 * @param format Pixel format to convert to
 */
void ConvertToFormat(uint8_t* bytes, int32_t len, PixelFormat format);

} // namespace ls
