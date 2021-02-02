/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * Base class for input devices, such as keyboard and gamepad.
 */
export class InputDevice {
  constructor ({ id, type, name, uuid }) {
    this._id = id
    this._type = type
    this._name = name
    this._uuid = uuid
    this.$connected = false
  }

  /**
   * Device instance ID.
   *
   * The instance ID identifies an input device belonging to a type. A keyboard device and a gamepad device
   * can both have the same instance Id, but two connected gamepads will never have share the same instance
   * ID. Also, if a device is connected, disconnected and reconnected, the device is not guaranteed to have
   * the same instance ID on reconnection.
   *
   * @return {string}
   */
  get id () {
    return this._id
  }

  /**
   * Device classification, such as keyboard or gamepad.
   *
   * @return {string}
   */
  get type () {
    return this._type
  }

  /**
   * Identifies a class or model of hardware.
   *
   * If two devices, of the exact same model, are connected, both devices will have the same UUID. The UUID
   * should not be used to attempt to uniquely identify a specific, physical device.
   *
   * @return {string}
   */
  get uuid () {
    return this._uuid
  }

  /**
   * UI displayable name of the device.
   *
   * The name of the device is reported by the input drivers. If no name exists for the device, an empty
   * string is returned. The name can be overridden through the Mapping API.
   *
   * @return {string}
   */
  get name () {
    return this._name
  }

  /**
   * Is the input device currently connected and broadcasting events?
   *
   * @returns {boolean}
   */
  get connected () {
    return this.$connected
  }

  /**
   * Checks if a mapped key is currently pressed down.
   *
   * Note, the state of the keyboard is updated every frame. So, this method returns the state of key from the
   * last time input events were processed (either last frame or beginning of current frame).
   *
   * @param key {Key} Mapped key.
   * @returns {boolean} true if key is pressed; otherwise, false
   */
  isKeyDown (key) {
    return false
  }

  /**
   * Gets the value of a mapped analog input, such as a control stick or trigger.
   *
   * Note, the state of the device is updated every frame. So, this method returns the state of analog input from the
   * last time input events were processed (either last frame or beginning of current frame).
   *
   * @param analog {AnalogKey} Analog input key.
   * @returns {number} value of the analog input
   */
  getAnalogValue (analog) {
    return 0
  }
}
