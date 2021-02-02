/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { now } from '../util/index.js'
import { EventName } from './EventName.js'

const EventSymbols = Object.entries(EventName).reduce((symbols, [key, value]) => {
  symbols[key] = Symbol.for(value)
  return symbols
}, {})

class Event {
  _state = -1

  constructor ($type, target, timestamp = now()) {
    this.$type = $type
    this.target = target
    this.currentTarget = target
    this.timestamp = timestamp
  }

  get type () {
    return Symbol.keyFor(this.$type)
  }

  stopPropagation () {
    if (this.cancelable) {
      this._state = 1
    }
  }

  stopImmediatePropagation () {
    if (this.cancelable) {
      this._state = 2
    }
  }

  get cancelable () {
    return this._state >= 0
  }

  hasStopPropagation () {
    return this._state > 0
  }

  hasStopImmediatePropagation () {
    return this._state === 2
  }
}

class StatusEvent extends Event {
  constructor (target, error = null) {
    super(EventSymbols.onStatus, target)
    this.error = error
  }
}

class GamepadEvent extends Event {
  constructor ($type, target, gamepad) {
    super($type, target)
    this.gamepad = gamepad
  }
}

class FocusChangeEvent extends Event {
  constructor ($type, target) {
    super($type, target)
    this._state = 0
  }
}

class ScanCodeEvent extends Event {
  constructor ($type, target, device, scanCode, pressed, repeat) {
    super($type, target)

    this.device = device
    this.pressed = pressed
    this.repeat = repeat
    this.scanCode = scanCode
    this._state = 0
  }
}

class ButtonEvent extends Event {
  constructor ($type, target, device, button, pressed, repeat) {
    super($type, target)

    this.device = device
    this.pressed = pressed
    this.repeat = repeat
    this.button = button
    this._state = 0
  }
}

class KeyEvent extends Event {
  constructor ($type, target, key, pressed, repeat) {
    super($type, target)

    this.key = key
    this.pressed = pressed
    this.repeat = repeat
    this._state = 0
  }
}

class AxisMotionEvent extends Event {
  constructor ($type, target, device, axis, value) {
    super($type, target)

    this.device = device
    this.axis = axis
    this.value = value
    this._state = 0
  }
}

class AnalogMotionEvent extends Event {
  constructor ($type, target, device, analogKey, value) {
    super($type, target)

    this.device = device
    this.analogKey = analogKey
    this.value = value
    this._state = 0
  }
}

class HatMotionEvent extends Event {
  constructor ($type, target, device, hat, value) {
    super($type, target)

    this.device = device
    this.hat = hat
    this.value = value
    this._state = 0
  }
}

export const createAttachedEvent = (target) => new Event(EventSymbols.onAttached, target)
export const createDetachedEvent = (target) => new Event(EventSymbols.onDetached, target)
export const createStartedEvent = (target) => new Event(EventSymbols.onStarted, target)
export const createStoppedEvent = (target) => new Event(EventSymbols.onStopped, target)
export const createDestroyedEvent = (target) => new Event(EventSymbols.onDestroyed, target)
export const createDestroyingEvent = (target) => new Event(EventSymbols.onDestroying, target)
export const createReadyStatusEvent = (target) => new StatusEvent(target)
export const createErrorStatusEvent = (target, error) => new StatusEvent(target, error)

export const createFocusEvent = (node) => new FocusChangeEvent(EventSymbols.onFocus, node)
export const createBlurEvent = (node) => new FocusChangeEvent(EventSymbols.onBlur, node)
export const createFocusInEvent = (node) => new FocusChangeEvent(EventSymbols.onFocusIn, node)
export const createFocusOutEvent = (node) => new FocusChangeEvent(EventSymbols.onFocusOut, node)

export const createGamepadConnectedEvent =
  (target, gamepad) => new GamepadEvent(EventSymbols.onConnected, target, gamepad)
export const createGamepadDisconnectedEvent =
  (target, gamepad) => new GamepadEvent(EventSymbols.onDisconnected, target, gamepad)

export const createScanCodeUpEvent =
  (target, device, scanCode) => new ScanCodeEvent(EventSymbols.onScanCodeUp, target, device, scanCode, false, false)
export const createScanCodeDownEvent =
  (target, device, scanCode, repeat) => new ScanCodeEvent(EventSymbols.onScanCodeDown, target, device, scanCode, true, repeat)
export const createButtonUpEvent =
  (target, device, button) => new ButtonEvent(EventSymbols.onButtonUp, target, device, button, false, false)
export const createButtonDownEvent =
  (target, device, button, repeat) => new ButtonEvent(EventSymbols.onButtonDown, target, device, button, true, repeat)
export const createAxisMotionEvent =
  (target, device, axis, value) => new AxisMotionEvent(EventSymbols.onAxisMotion, target, device, axis, value)
export const createHatMotionEvent =
  (target, device, hat, value) => new HatMotionEvent(EventSymbols.onHatMotion, target, device, hat, value)
export const createKeyUpEvent =
  (target, key) => new KeyEvent(EventSymbols.onKeyUp, target, key, false, false)
export const createKeyDownEvent =
  (target, key, repeat) => new KeyEvent(EventSymbols.onKeyDown, target, key, true, repeat)
export const createAnalogMotionEvent =
  (target, device, axis, value) => new AnalogMotionEvent(EventSymbols.onAnalogMotion, target, device, axis, value)
