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
  console.log('Error loading light-source.node: ' + e.message)
  lib = {
    StyleBase: () => {},
    StyleEnumMappings: () => {},
    SceneBase: () => {},
    StageBase: () => {},
    FontStoreView: () => {},
    ImageStoreView: () => {},
    BoxSceneNode: () => {},
    ImageSceneNode: () => {},
    RootSceneNode: () => {},
    TextSceneNode: () => {},
    getSceneNodeInstanceCount: () => 0,
    addonError: e,
  }
}

export const {
  StyleBase,
  StyleEnumMappings,
  SceneBase,
  StageBase,
  FontStoreView,
  ImageStoreView,
  BoxSceneNode,
  ImageSceneNode,
  RootSceneNode,
  TextSceneNode,
  getSceneNodeInstanceCount,
  addonError
} = lib

export const SDLModuleId = 'light-source-sdl'
export const SDLMixerModuleId = 'light-source-sdl-mixer'

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
