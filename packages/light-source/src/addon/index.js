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

const emptyFunction = () => 0

const CreateStubStageBase = () =>
  class StubStageBase {
    $destroy() {}
  }

const CreateStubStyle = () =>
  class StubStyle {
  }

const CreateStubStyleValue = () =>
  class StubStyleValue {
    unit = 0
    value = 0
    static of (value) { return new StubStyleValue() }
  }

const CreateStubStyleTransformSpec = () =>
  class StubStyleTransformSpec {
    transform = -1
    static identity () {}
    static rotate (angle) {}
    static scale (x, y) {}
    static translate (x, y) {}
    static validate(spec) { return false }
  }

const CreateStubStyleClass = () =>
  class StubStyleClass {
  }

const CreateStubLogger = () => ({
  warn(message, site) {},
  info(message, site) {},
  error(message, site) {},
  debug(message, site) {},
  setLogLevel(logLevel) {},
  getLogLevel() { return -1 }
})

export const Style = lib.Style || CreateStubStyle()
export const StyleClass = lib.StyleClass || CreateStubStyleClass()
export const StyleUnit = lib.StyleUnit || {}
export const StyleTransform = lib.StyleTransform || {}
export const StyleTransformSpec = lib.StyleTransformSpec || {}
export const StyleValue = lib.StyleValue || CreateStubStyleValue()
export const SceneBase = lib.SceneBase || class {}
export const StageBase = lib.StageBase || CreateStubStageBase()
export const logger = lib.logger || CreateStubLogger()
export const LogLevel  = lib.LogLevel || {}
export const getSceneNodeInstanceCount = lib.getSceneNodeInstanceCount || emptyFunction
export const parseColor = lib.parseColor || emptyFunction
export const styleProperties = lib.styleProperties || {}

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
