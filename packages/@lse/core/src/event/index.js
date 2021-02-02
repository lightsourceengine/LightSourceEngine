/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { now } from '../util/index.js'

export const EventNames = {
  attached: 'attached',
  detached: 'detached',
  started: 'started',
  stopped: 'stopped',
  destroying: 'destroying',
  destroyed: 'destroyed',
  status: 'status',
  connected: 'connected',
  disconnected: 'disconnected',
  focus: 'focus',
  blur: 'blur',
  focusin: 'focusin',
  focusout: 'focusout',
  rawkeyup: 'rawkeyup',
  rawkeydown: 'rawkeydown',
  rawbuttonup: 'rawbuttonup',
  rawbuttondown: 'rawbuttondown',
  rawhatmotion: 'rawhatmotion',
  rawaxismotion: 'rawaxismotion',
  keyup: 'keyup',
  keydown: 'keydown',
  analogmotion: 'analogmotion'
}

const EventSymbols = {
  attached: Symbol.for(EventNames.attached),
  detached: Symbol.for(EventNames.detached),
  started: Symbol.for(EventNames.started),
  stopped: Symbol.for(EventNames.stopped),
  destroying: Symbol.for(EventNames.destroying),
  destroyed: Symbol.for(EventNames.destroyed),
  status: Symbol.for(EventNames.status),
  connected: Symbol.for(EventNames.connected),
  disconnected: Symbol.for(EventNames.disconnected),
  focus: Symbol.for(EventNames.focus),
  blur: Symbol.for(EventNames.blur),
  focusin: Symbol.for(EventNames.focusin),
  focusout: Symbol.for(EventNames.focusout),
  rawkeyup: Symbol.for(EventNames.rawkeyup),
  rawkeydown: Symbol.for(EventNames.rawkeydown),
  rawbuttonup: Symbol.for(EventNames.rawbuttonup),
  rawbuttondown: Symbol.for(EventNames.rawbuttondown),
  rawhatmotion: Symbol.for(EventNames.rawhatmotion),
  rawaxismotion: Symbol.for(EventNames.rawaxismotion),
  keyup: Symbol.for(EventNames.keyup),
  keydown: Symbol.for(EventNames.keydown),
  analogmotion: Symbol.for(EventNames.analogmotion)
}

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

class RawKeyEvent extends Event {
  constructor ($type, target, device, scanCode, pressed, repeat) {
    super($type, target)

    this.device = device
    this.pressed = pressed
    this.repeat = repeat
    this.scanCode = scanCode
    this._state = 0
  }
}

class RawButtonEvent extends Event {
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

class RawAxisEvent extends Event {
  constructor ($type, target, device, axis, value) {
    super($type, target)

    this.device = device
    this.axis = axis
    this.value = value
    this._state = 0
  }
}

class AnalogEvent extends Event {
  constructor ($type, target, device, analogKey, value) {
    super($type, target)

    this.device = device
    this.analogKey = analogKey
    this.value = value
    this._state = 0
  }
}

class RawHatEvent extends Event {
  constructor ($type, target, device, hat, value) {
    super($type, target)

    this.device = device
    this.hat = hat
    this.value = value
    this._state = 0
  }
}

export const AttachedEvent = (target) => new Event(EventSymbols.attached, target)
export const DetachedEvent = (target) => new Event(EventSymbols.detached, target)
export const StartedEvent = (target) => new Event(EventSymbols.started, target)
export const StoppedEvent = (target) => new Event(EventSymbols.stopped, target)
export const DestroyedEvent = (target) => new Event(EventSymbols.destroyed, target)
export const DestroyingEvent = (target) => new Event(EventSymbols.destroying, target)
export const GamepadConnectedEvent =
  (target, gamepad) => new GamepadEvent(EventSymbols.connected, target, gamepad)
export const GamepadDisconnectedEvent =
  (target, gamepad) => new GamepadEvent(EventSymbols.disconnected, target, gamepad)
export const ReadyStatusEvent = (target) => new StatusEvent(target)
export const ErrorStatusEvent = (target, error) => new StatusEvent(target, error)
export const FocusEvent = (node) => new FocusChangeEvent(EventSymbols.focus, node)
export const BlurEvent = (node) => new FocusChangeEvent(EventSymbols.blur, node)
export const FocusInEvent = (node) => new FocusChangeEvent(EventSymbols.focusin, node)
export const FocusOutEvent = (node) => new FocusChangeEvent(EventSymbols.focusout, node)
export const RawKeyUpEvent =
  (target, device, scanCode) => new RawKeyEvent(EventSymbols.rawkeyup, target, device, scanCode, false, false)
export const RawKeyDownEvent =
  (target, device, scanCode, repeat) => new RawKeyEvent(EventSymbols.rawkeydown, target, device, scanCode, true, repeat)
export const RawButtonUpEvent =
  (target, device, button) => new RawButtonEvent(EventSymbols.rawbuttonup, target, device, button, false, false)
export const RawButtonDownEvent =
  (target, device, button, repeat) => new RawButtonEvent(EventSymbols.rawbuttondown, target, device, button, true, repeat)
export const RawAxisMotionEvent =
  (target, device, axis, value) => new RawAxisEvent(EventSymbols.rawaxismotion, target, device, axis, value)
export const RawHatMotionEvent =
  (target, device, hat, value) => new RawHatEvent(EventSymbols.rawhatmotion, target, device, hat, value)
export const KeyUpEvent =
  (target, key) => new KeyEvent(EventSymbols.keyup, target, key, false, false)
export const KeyDownEvent =
  (target, key, repeat) => new KeyEvent(EventSymbols.keydown, target, key, true, repeat)
export const AnalogMotionEvent =
  (target, device, axis, value) => new AnalogEvent(EventSymbols.analogmotion, target, device, axis, value)
