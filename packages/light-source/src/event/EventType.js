/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

export const EventType = Object.freeze({
  KeyDown: Symbol.for('KeyDown'),
  KeyUp: Symbol.for('KeyUp'),
  AxisMotion: Symbol.for('AxisMotion'),

  DeviceButtonDown: Symbol.for('DeviceButtonDown'),
  DeviceButtonUp: Symbol.for('DeviceButtonUp'),
  DeviceHatDown: Symbol.for('DeviceHatDown'),
  DeviceHatUp: Symbol.for('DeviceHatUp'),
  DeviceAxisMotion: Symbol.for('DeviceAxisMotion'),

  DeviceConnected: Symbol.for('DeviceConnected'),
  DeviceDisconnected: Symbol.for('DeviceDisconnected'),

  Focus: Symbol.for('Focus'),
  Blur: Symbol.for('Blur')
})
