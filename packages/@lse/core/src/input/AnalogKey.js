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

/**
 * Enums for identifying the analog inputs (axis, trigger) of a gamepad.
 *
 * @see Key
 * @enum {number}
 * @readonly
 * @name module:@lse/core.core-enum.AnalogKey
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
