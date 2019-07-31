/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Renderer.h>

namespace ls {

extern const bool isBigEndian;

void ConvertToFormat(uint8_t* bytes, int32_t len, PixelFormat format);

} // namespace ls
