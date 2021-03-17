/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * Flags for to help interpret the state of a gamepad hat (directional pad) value.
 *
 * @enum {number}
 * @readonly
 * @name module:@lse/core.core-enum.Hat
 */
export const Hat = Object.freeze({
  CENTERED: 0,
  UP: 0x01,
  RIGHT: 0x02,
  DOWN: 0x04,
  LEFT: 0x08
})
