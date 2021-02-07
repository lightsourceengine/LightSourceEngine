/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { createRequire } from 'module'
import { resolve } from 'path'
import bindings from 'bindings'

// Load the native portions of Light Source Engine from @lse/core.

const { LSE_ENV, LSE_PATH } = process.env
const NATIVE_MODULE_NAME = 'lse-core.node'
const require = createRequire(import.meta.url)
let addon = null

try {
  if (LSE_ENV === 'lse-node') {
    // In "lse-node" mode, load the native module from the pseudo-builtin module path.
    addon = require(resolve(LSE_PATH, '@lse', 'core', 'Release', NATIVE_MODULE_NAME))
  } else {
    // In "node" mode, load the native module from the package manager installed node_modules.
    addon = bindings(NATIVE_MODULE_NAME)
  }
} catch (e) {
  console.error(`ERROR: Cannot load native code from ${NATIVE_MODULE_NAME}: ${e.message}`)
  process.exit(1)
}

// Class Exports
export const {
  CStage,
  CScene,
  CFontManager,
  Style,
  StyleClass,
  StyleTransformSpec,
  StyleValue,
  BoxSceneNode:BoxSceneNodeBase,
  ImageSceneNode:ImageSceneNodeBase,
  LinkSceneNode:LinkSceneNodeBase,
  RootSceneNode:RootSceneNodeBase,
  TextSceneNode:TextSceneNodeBase
} = addon

// Enum Exports
export const {
  StyleUnit,
  StyleTransform,
  StyleAnchor,
  LogLevel,
  FontStatus,
  FontStyle,
  FontWeight
} = addon

// Function / Object Exports
export const {
  logger,
  getSceneNodeInstanceCount,
  parseColor,
  loadSDLPlugin,
  loadSDLAudioPlugin,
  loadSDLMixerPlugin,
  loadRefPlugin,
  styleProperties,
  createRefGraphicsContext,
} = addon
