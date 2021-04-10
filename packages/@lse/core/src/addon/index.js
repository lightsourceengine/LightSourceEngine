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
  CStyle,
  CFontManager,
  CImageManager,
  CBoxSceneNode,
  CImageSceneNode,
  CRootSceneNode,
  CTextSceneNode
} = addon

// Enum Exports
export const {
  /**
   * @enum {number}
   * @readonly
   * @name module:@lse/core.core-enum.StyleUnit
   */
  StyleUnit,
  /**
   * @enum {number}
   * @readonly
   * @name module:@lse/core.core-enum.StyleTransform
   */
  StyleTransform,
  /**
   * @enum {number}
   * @readonly
   * @name module:@lse/core.core-enum.StyleAnchor
   */
  StyleAnchor,
  /**
   * @enum {number}
   * @readonly
   * @name module:@lse/core.core-enum.StyleFilter
   */
  StyleFilter,
  /**
   * @enum {number}
   * @readonly
   * @name module:@lse/core.core-enum.LogLevel
   */
  LogLevel,
  FontStatus,
  FontStyle,
  FontWeight
} = addon

// Function / Object Exports
export const {
  /**
   * Logger.
   *
   * @type {object}
   * @name module:@lse/core.logger
   */
  logger,
  getSceneNodeInstanceCount,
  parseColor,
  parseValue,
  lockStyle,
  resetStyle,
  setStyleParent,
  loadSDLPlugin,
  loadRefPlugin,
  loadSDLAudioPlugin,
  loadSDLMixerPlugin,
  installStyleValue,
  installStyleTransformSpec,
  installImage
} = addon
