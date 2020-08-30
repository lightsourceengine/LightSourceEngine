/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import bindings from 'bindings'

let lib
let error

try {
  lib = bindings('light-source')
} catch (e) {
  console.log('Error loading light-source.node: ' + e.message)
  lib = {}
  error = e
}

const EmptyClass = class {}
const emptyFunction = () => 0

const StubStageBaseClass = () =>
  class StubStageBase {
    $destroy() {}
  }

const StubStyleClass = () =>
  class StubStyle {
  }

const StubLogger = () => ({
  warn(message, site) {},
  info(message, site) {},
  error(message, site) {},
  debug(message, site) {},
  setLogLevel(logLevel) {},
  getLogLevel() { return -1 }
})

export const Style = lib.Style || StubStyleClass()
export const StyleUnit = lib.StyleUnit || {}
export const StyleTransform = lib.StyleTransform || {}
export const SceneBase = lib.SceneBase || EmptyClass
export const StageBase = lib.StageBase || StubStageBaseClass()
export const logger = lib.Logger || StubLogger()
export const LogLevel  = lib.LogLevel || {}
export const getSceneNodeInstanceCount = lib.getSceneNodeInstanceCount || emptyFunction
export const parseColor = lib.parseColor || emptyFunction

export const addon = lib
export const addonError = error

/**
 * @class SceneNode
 */

/**
 * @class BoxSceneNode
 */

/**
 * @class ImageSceneNode
 */

/**
 * @class TextSceneNode
 */

/**
 * @class Keyboard
 */

/**
 * @class Gamepad
 */

/**
 * @class Display
 */
