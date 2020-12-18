/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import bindings from 'bindings'
import { join, dirname, basename } from 'path'
import { createRequire } from 'module'
import { fileURLToPath } from 'url'

/**
 * Load the native portions of Light Source Engine from light-source.node.
 *
 * When Light Source Engine is packaged as a runtime with the light-source module as a builtin, light-source.node
 * must be loaded from the builtin path. The builtin path is hard coded in this file because node's import does not
 * have native addon support yet.
 *
 * In all other environments, light-source.node is loaded through bindings. The bindings module knows how to find
 * the correct Release or Debug .node in the node_module directories.
 *
 * When light-source is bundled, global.LS_BINDINGS_TYPE is set. npm bundles get "bindings" and standalone bundles
 * get "builtin-bindings".
 *
 * @ignore
 */
export const loadLightSourceAddon = () => {
  const name = 'light-source.node'
  let addon = null
  let addonError = null

  try {
    if (global.LS_BINDINGS_TYPE === 'builtin-bindings') {
      // TODO: would rather use import and light-source-loader/builtin, but node es modules do not support native addons yet
      const { url } = import.meta
      const module = getLightSourceModuleFromUrl(url)
      const require = createRequire(url)

      addon = require(join(module, 'Release', name))
    } else {
      addon = bindings(name)
    }
  } catch (e) {
    console.error('Failed to load light-source.node: ' + e.message)
    addonError = e
  }

  return [ addon, addonError ]
}

const getLightSourceModuleFromUrl = url => {
  const module = dirname(fileURLToPath(url))

  if (basename(module) !== 'light-source') {
    throw Error('Light Source Engine js code is not in packaged runtime')
  }

  return module
}
