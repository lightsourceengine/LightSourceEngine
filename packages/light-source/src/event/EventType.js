/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { symbolFor } from '../util'

export const EventType = {
  KeyDown: symbolFor('KeyDown'),
  KeyUp: symbolFor('KeyUp'),
  AxisMotion: symbolFor('AxisMotion'),

  DeviceButtonDown: symbolFor('DeviceButtonDown'),
  DeviceButtonUp: symbolFor('DeviceButtonUp'),
  DeviceHatDown: symbolFor('DeviceHatDown'),
  DeviceHatUp: symbolFor('DeviceHatUp'),
  DeviceAxisMotion: symbolFor('DeviceAxisMotion'),

  DeviceConnected: symbolFor('DeviceConnected'),
  DeviceDisconnected: symbolFor('DeviceDisconnected'),

  Focus: symbolFor('Focus'),
  Blur: symbolFor('Blur')
}
