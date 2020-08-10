/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#define LS_LITTLE_ENDIAN 0
#define LS_BIG_ENDIAN 1

#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
#define LS_BYTE_ORDER LS_BIG_ENDIAN
#else
#define LS_BYTE_ORDER LS_LITTLE_ENDIAN
#endif
