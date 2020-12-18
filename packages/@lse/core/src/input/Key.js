/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * Mapped key codes for a 'standard' input device. This mapping closely matches the standard mapping of the Gamepad
 * specification (https://w3c.github.io/gamepad/#remapping).
 */
export const Key = {
  B: 0,
  A: 1,
  Y: 2,
  X: 3,

  L1: 4,
  R1: 5,
  L2: 6,
  R2: 7,

  SELECT: 8, // back
  START: 9, // forward

  LS: 10,
  RS: 11,

  UP: 12,
  DOWN: 13,
  LEFT: 14,
  RIGHT: 15,

  HOME: 16

  // TODO: implement axis handling
  // LS_UP: 17,
  // LS_DOWN: 18,
  // LS_LEFT: 19,
  // LS_RIGHT: 20,
  //
  // RS_UP: 21,
  // RS_DOWN: 22,
  // RS_LEFT: 23,
  // RS_RIGHT: 24
}
