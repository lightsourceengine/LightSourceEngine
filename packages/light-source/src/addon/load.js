/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import bindings from 'bindings'
import { join, resolve } from 'path'

/**
 * Wrapper around bindings for loading .node files.
 *
 * For development, bindings will be used to load .node files from node_modules. When running with ls-node,
 * modules will be loaded from the light-source package in LS_ADDON_PATH.
 *
 * @ignore
 */
export const load = (opts) => {
  if (process.env.LS_BINDINGS === 'custom-bindings') {
    return customBindings(opts)
  } else {
    return bindings(opts)
  }
}

const customBindings = (opts) => {
  if (typeof opts == 'string') {
    opts = { bindings: opts };
  } else if (!opts) {
    opts = {};
  }

  if (typeof opts.bindings !== 'string') {
    opts.bindings = 'bindings.node'
  }

  if (!opts.bindings.endsWith('.node')) {
    opts.bindings += '.node'
  }

  let { LS_ADDON_PATH } = process.env

  if (LS_ADDON_PATH) {
    LS_ADDON_PATH = resolve(LS_ADDON_PATH)
  }

  if (!LS_ADDON_PATH) {
    throw Error('No bindings search path set. Expected LS_ADDON_PATH.')
  }

  return require(join(LS_ADDON_PATH, opts.bindings))
}
