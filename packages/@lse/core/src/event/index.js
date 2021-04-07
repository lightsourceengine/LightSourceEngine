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
  constructor (target, error = undefined) {
    super(EventSymbols.status, target)
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

export const createAttachedEvent = (target) => new Event(EventSymbols.attached, target)
export const createDetachedEvent = (target) => new Event(EventSymbols.detached, target)
export const createStartedEvent = (target) => new Event(EventSymbols.started, target)
export const createStoppedEvent = (target) => new Event(EventSymbols.stopped, target)
export const createDestroyedEvent = (target) => new Event(EventSymbols.destroyed, target)
export const createDestroyingEvent = (target) => new Event(EventSymbols.destroying, target)
export const createReadyStatusEvent = (target) => new StatusEvent(target)
export const createErrorStatusEvent = (target, error) => new StatusEvent(target, error)

export const createFocusEvent = (node) => new FocusChangeEvent(EventSymbols.focus, node)
export const createBlurEvent = (node) => new FocusChangeEvent(EventSymbols.blur, node)
export const createFocusInEvent = (node) => new FocusChangeEvent(EventSymbols.focusIn, node)
export const createFocusOutEvent = (node) => new FocusChangeEvent(EventSymbols.focusOut, node)

export const createGamepadConnectedEvent =
  (target, gamepad) => new GamepadEvent(EventSymbols.connected, target, gamepad)
export const createGamepadDisconnectedEvent =
  (target, gamepad) => new GamepadEvent(EventSymbols.disconnected, target, gamepad)

export const createScanCodeUpEvent =
  (target, device, scanCode) => new ScanCodeEvent(EventSymbols.scanCodeUp, target, device, scanCode, false, false)
export const createScanCodeDownEvent =
  (target, device, scanCode, repeat) => new ScanCodeEvent(EventSymbols.scanCodeDown, target, device, scanCode, true, repeat)
export const createButtonUpEvent =
  (target, device, button) => new ButtonEvent(EventSymbols.buttonUp, target, device, button, false, false)
export const createButtonDownEvent =
  (target, device, button, repeat) => new ButtonEvent(EventSymbols.buttonDown, target, device, button, true, repeat)
export const createAxisMotionEvent =
  (target, device, axis, value) => new AxisMotionEvent(EventSymbols.axisMotion, target, device, axis, value)
export const createHatMotionEvent =
  (target, device, hat, value) => new HatMotionEvent(EventSymbols.hatMotion, target, device, hat, value)
export const createKeyUpEvent =
  (target, key) => new KeyEvent(EventSymbols.keyUp, target, key, false, false)
export const createKeyDownEvent =
  (target, key, repeat) => new KeyEvent(EventSymbols.keyDown, target, key, true, repeat)
export const createAnalogMotionEvent =
  (target, device, axis, value) => new AnalogMotionEvent(EventSymbols.analogMotion, target, device, axis, value)
