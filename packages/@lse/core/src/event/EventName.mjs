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
 * Event names used by EventTargets in @lse/core.
 *
 * @enum {string}
 * @readonly
 * @name module:@lse/core.core-enum.EventName
 */
export const EventName = Object.freeze({
  /// //////////////////////////////////////////////////////////////////////////
  // Lifecycle Events (Scene, Stage, etc)
  /// //////////////////////////////////////////////////////////////////////////

  attached: 'attached',
  detached: 'detached',
  started: 'started',
  stopped: 'stopped',
  destroying: 'destroying',
  destroyed: 'destroyed',
  status: 'status',

  /// //////////////////////////////////////////////////////////////////////////
  // Scene Graph Events
  /// //////////////////////////////////////////////////////////////////////////

  focus: 'focus',
  blur: 'blur',
  focusIn: 'focus-in',
  focusOut: 'focus-out',

  /// //////////////////////////////////////////////////////////////////////////
  // Input Device Connection Events
  /// //////////////////////////////////////////////////////////////////////////

  connected: 'connected',
  disconnected: 'disconnected',

  /// //////////////////////////////////////////////////////////////////////////
  // Mapped Input Events (Key and Analog based events)
  /// //////////////////////////////////////////////////////////////////////////

  keyUp: 'key-up',
  keyDown: 'key-down',
  analogMotion: 'analog-motion',

  /// //////////////////////////////////////////////////////////////////////////
  // Raw Hardware Input Events
  /// //////////////////////////////////////////////////////////////////////////

  scanCodeUp: 'scan-code-up',
  scanCodeDown: 'scan-code-down',
  buttonUp: 'button-up',
  buttonDown: 'button-down',
  hatMotion: 'hat-motion',
  axisMotion: 'axis-motion'
})
