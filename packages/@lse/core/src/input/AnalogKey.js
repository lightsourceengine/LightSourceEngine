/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * Enums for identifying the analog inputs (axis, trigger) of a gamepad.
 *
 * @type {{LEFT_STICK_Y: number, LEFT_TRIGGER: number, LEFT_STICK_X: number, RIGHT_STICK_X: number, RIGHT_STICK_Y: number, RIGHT_TRIGGER: number}}
 * @see Key
 */
export const AnalogKey = {
  // analog sticks
  LEFT_STICK_X: 1000,
  LEFT_STICK_Y: 1001,
  RIGHT_STICK_X: 1002,
  RIGHT_STICK_Y: 1003,
  // analog triggers
  LEFT_TRIGGER: 1004,
  RIGHT_TRIGGER: 1005
}
