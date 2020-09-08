/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import bindings from 'bindings'
import { join, delimiter } from 'path'

/**
 * Wrapper around bindings for loading .node files.
 *
 * For development, bindings will be used to load .node files from node_modules. When running with ls-node,
 * modules will be loaded from the light-source package in NODE_PATH.
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

  let { NODE_PATH = '' } = process.env
  let tried = ''

  for (let searchPath of NODE_PATH.split(delimiter)) {
    searchPath = searchPath.trim()

    if (!searchPath) {
      continue
    }

    const candidate = join(searchPath, 'light-source', 'Release', opts.bindings)

    try {
      return require(candidate)
    } catch (e) {
      tried += `\n${candidate}: ${e.message}`
    }
  }

  throw Error(`Could not find ${opts.bindings} in $NODE_PATH. Tried:${tried}`)
}
