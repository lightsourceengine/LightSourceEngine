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

export const Style = lib.Style || EmptyClass
export const SceneBase = lib.SceneBase || EmptyClass
export const StageBase = lib.StageBase || EmptyClass
export const FontStoreView = lib.FontStoreView || EmptyClass
export const ImageStoreView = lib.ImageStoreView || EmptyClass
export const BoxSceneNode = lib.BoxSceneNode || EmptyClass
export const ImageSceneNode = lib.ImageSceneNode || EmptyClass
export const RootSceneNode = lib.RootSceneNode || EmptyClass
export const TextSceneNode = lib.TextSceneNode || EmptyClass
export const getSceneNodeInstanceCount = lib.getSceneNodeInstanceCount || emptyFunction
export const Logger = lib.Logger || {}
export const parseColor = lib.parseColor || EmptyClass
export const addonError = error

export const SDLModuleId = 'light-source-sdl'
export const SDLAudioModuleId = 'light-source-sdl-audio'
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
