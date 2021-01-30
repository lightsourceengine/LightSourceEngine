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

// Load lse-core.node. If this operation fails, populate the light-source javascript api with stub implementations
// so the app can continue to load. When the stage is init'd, the user should be notified of the native addon
// load failure and deal with the error in their app.
export const [ addon, addonError ] = loadLightSourceAddon()

export const {
  Style = (class StubStyle {}),
  StyleClass = (class StubStyleClass {}),
  StyleUnit = {},
  StyleTransform = {},
  StyleTransformSpec = StubStyleTransformSpec,
  StyleAnchor = {},
  StyleValue = StubStyleValue,
  PluginId = {},
  SceneBase = (class SceneBase {}),
  StageBase = StubStageBase,
  logger = CreateStubLogger(),
  LogLevel = {},
  getSceneNodeInstanceCount = (() => 0),
  parseColor = (() => 0),
  loadPluginById = ((id) => { throw Error('native code not loaded') }),
  styleProperties = {}
} = addon

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
