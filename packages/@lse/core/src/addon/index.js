/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { loadLightSourceAddon } from './loadLightSourceAddon.js'

class StubStageBase {
  $destroy() {}
}

class StubStyleValue {
  unit = 0
  value = 0
  static of (value) { return new StubStyleValue() }
}

class StubStyleTransformSpec {
  transform = -1
  static identity () {}
  static rotate (angle) {}
  static scale (x, y) {}
  static translate (x, y) {}
  static validate(spec) { return false }
}

const CreateStubLogger = () => ({
  warn(message, site) {},
  info(message, site) {},
  error(message, site) {},
  debug(message, site) {},
  setLogLevel(logLevel) {},
  getLogLevel() { return -1 }
})

// Load light-source.node. If this operation fails, populate the light-source javascript api with stub implementations
// so the app can continue to load. When the stage is init'd, the user should be notified of the native addon
// load failure and deal with the error in their app.
export const [ addon, addonError ] = loadLightSourceAddon()

export const Style = addon.Style || (class StubStyle {})
export const StyleClass = addon.StyleClass || (class StubStyleClass {})
export const StyleUnit = addon.StyleUnit || {}
export const StyleTransform = addon.StyleTransform || {}
export const StyleTransformSpec = addon.StyleTransformSpec || StubStyleTransformSpec
export const StyleAnchor = addon.StyleAnchor || {}
export const StyleValue = addon.StyleValue || StubStyleValue
export const SceneBase = addon.SceneBase || (class SceneBase {})
export const StageBase = addon.StageBase || StubStageBase
export const logger = addon.logger || CreateStubLogger()
export const LogLevel  = addon.LogLevel || {}
export const getSceneNodeInstanceCount = addon.getSceneNodeInstanceCount || (() => 0)
export const parseColor = addon.parseColor || (() => 0)
export const loadPlugin = addon.loadPlugin || ((id) => { throw Error('native code not loaded') })
export const styleProperties = addon.styleProperties || {}

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
