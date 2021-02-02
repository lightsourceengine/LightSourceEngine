/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * Event names used by EventTargets in @lse/core.
 */
export const EventName = Object.freeze({
  /// //////////////////////////////////////////////////////////////////////////
  // Lifecycle Events (Scene, Stage, etc)
  /// //////////////////////////////////////////////////////////////////////////

  onAttached: 'on-attached',
  onDetached: 'on-detached',
  onStarted: 'on-started',
  onStopped: 'on-stopped',
  onDestroying: 'on-destroying',
  onDestroyed: 'on-destroyed',
  onStatus: 'on-status',

  /// //////////////////////////////////////////////////////////////////////////
  // Scene Graph Events
  /// //////////////////////////////////////////////////////////////////////////

  onFocus: 'on-focus',
  onBlur: 'on-blur',
  onFocusIn: 'on-focus-in',
  onFocusOut: 'on-focus-out',

  /// //////////////////////////////////////////////////////////////////////////
  // Input Device Connection Events
  /// //////////////////////////////////////////////////////////////////////////

  onConnected: 'on-connected',
  onDisconnected: 'on-disconnected',

  /// //////////////////////////////////////////////////////////////////////////
  // Mapped Input Events (Key and Analog based events)
  /// //////////////////////////////////////////////////////////////////////////

  onKeyUp: 'on-key-up',
  onKeyDown: 'on-key-down',
  onAnalogMotion: 'on-analog-motion',

  /// //////////////////////////////////////////////////////////////////////////
  // Raw Hardware Input Events
  /// //////////////////////////////////////////////////////////////////////////

  onScanCodeUp: 'on-scan-code-up',
  onScanCodeDown: 'on-scan-code-down',
  onButtonUp: 'on-button-up',
  onButtonDown: 'on-button-down',
  onHatMotion: 'on-hat-motion',
  onAxisMotion: 'on-axis-motion'
})
