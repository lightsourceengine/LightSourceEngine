/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import bindings from 'bindings'

let lib

try {
  lib = bindings('light-source')
} catch (e) {
  lib.addonError = e
  lib.StyleBase = () => {}
  lib.StyleEnumMappings = () => {}
  lib.SceneBase = () => {}
  lib.BoxSceneNode = () => {}
  lib.ImageSceneNode = () => {}
  lib.TextSceneNode = () => {}
}

export const {
  StyleBase,
  StyleEnumMappings,
  SceneBase,
  BoxSceneNode,
  ImageSceneNode,
  TextSceneNode,
  addonError
} = lib

/**
 * @class ResourceManager
 */

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
