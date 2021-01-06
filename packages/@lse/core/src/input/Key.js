/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * Enums for identifying the digital inputs (buttons) of a gamepad.
 *
 * @type {{A: number, B: number, LEFT_SHOULDER: number, LEFT_STICK: number, GUIDE: number, RIGHT_STICK: number, DPAD_UP: number, DPAD_DOWN: number, DPAD_RIGHT: number, X: number, Y: number, BACK: number, START: number, RIGHT_SHOULDER: number, DPAD_LEFT: number}}
 * @see AnalogKey
 */
export const Key = {
  A: 0,
  B: 1,
  X: 2,
  Y: 3,
  BACK: 4,
  GUIDE: 5,
  START: 6,
  LEFT_STICK: 7,
  RIGHT_STICK: 8,
  LEFT_SHOULDER: 9,
  RIGHT_SHOULDER: 10,
  DPAD_UP: 11,
  DPAD_DOWN: 12,
  DPAD_LEFT: 13,
  DPAD_RIGHT: 14
}
