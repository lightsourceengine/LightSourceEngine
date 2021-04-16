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

import { InputDevice } from './InputDevice.mjs'

/**
 * Gamepad input device.
 *
 * @memberof module:@lse/core
 * @extends module:@lse/core.InputDevice
 * @hideconstructor
 */
class Gamepad extends InputDevice {
  constructor (id, api) {
    const info = api.getGamepadInfo(id)

    super(info)

    this._api = api
    this._info = info
  }

  /**
   * Number of buttons on the gamepad.
   *
   * @return {int}
   */
  get buttonCount () {
    return this._info.buttonCount
  }

  /**
   * Number of analog axes on the gamepad.
   *
   * @return {int}
   */
  get axisCount () {
    return this._info.axisCount
  }

  /**
   * Number of hats available on the gamepad. Hats are typically used to represent a directional pad
   * on a device.
   *
   * @return {int}
   */
  get hatCount () {
    return this._info.hatCount
  }

  /**
   * @override
   */
  isKeyDown (key) {
    return this._api.isKeyDown(this.id, key)
  }

  /**
   * @override
   */
  getAnalogValue (analogKey) {
    return this._api.getAnalogValue(this.id, analogKey)
  }

  /**
   * Gets the state (pressed or released) of a gamepad button.
   *
   * @param button {int} Hardware button id in the range of [0, buttonCount).
   * @returns {boolean} true if button is currently pressed, false otherwise
   */
  getButtonState (button) {
    return this._api.getButtonState(this.id, button)
  }

  /**
   * Gets the state of a gamepad axis.
   *
   * @param axis {int} Hardware axis id in the range of [0, axisCount).
   * @returns {number} value in the range of [-1,1]. for all error conditions, 0 is returned
   */
  getAxisState (axis) {
    return this._api.getAxisState(this.id, axis)
  }

  /**
   * Gets the state of a gamepad hat (directional pad).
   *
   * Use the flags from Hat to interpret value returned by this function.
   *
   * @param hat {int} Hardware hat id in the range of [0, hatCount).
   * @returns {number} hat value. for all error conditions, Hat.CENTERED is returned
   * @see Hat
   */
  getHatState (hat) {
    return this._api.getHatState(this.id, hat)
  }
}

export { Gamepad }
