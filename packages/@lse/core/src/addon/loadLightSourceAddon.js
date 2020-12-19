/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import bindings from 'bindings'
import { resolve } from 'path'
import { createRequire } from 'module'

const { LSE_ENV, LSE_PATH } = process.env
const NATIVE_MODULE_NAME = 'light-source.node'
const require = createRequire(import.meta.url)

/**
 * Load the native portions of Light Source Engine from @lse/core.
 *
 * @ignore
 */
export const loadLightSourceAddon = () => {
  let addon = null
  let addonError = null

  try {
    if (LSE_ENV === 'lse-node') {
      // In "lse-node" mode, load the native module from the pseudo-builtin module path.
      addon = require(resolve(LSE_PATH, '@lse', 'core', 'Release', NATIVE_MODULE_NAME))
    } else {
      // In "node" mode, load the native module from the package manager installed node_modules.
      addon = bindings(NATIVE_MODULE_NAME)
    }
  } catch (e) {
    console.error(`Failed to load ${NATIVE_MODULE_NAME}: ${e.message}`)
    addonError = e
  }

  return [ addon, addonError ]
}
