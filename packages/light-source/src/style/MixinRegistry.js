/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { absoluteFill } from './absoluteFill'

const kMixinMap = new Map()

export class MixinRegistry {
  static add (id, spec) {
    if (!id || typeof id !== 'string' || !id.startsWith('%')) {
      throw Error(`id must be a string that starts with '%' (${id})`)
    }

    if (!spec || typeof spec !== 'object') {
      throw Error('expand must be a plain object')
    }

    if (kMixinMap.has(id)) {
      throw Error(`Mixin '${id}' already exists`)
    }

    kMixinMap.set(id, spec)
  }

  static get (id) {
    return kMixinMap.get(id)
  }

  static delete (id) {
    kMixinMap.delete(id)
  }

  static has (id) {
    return kMixinMap.has(id)
  }
}

export const resetMixinRegistry = () => {
  kMixinMap.clear()
  kMixinMap.set('%absoluteFill', absoluteFill)
}

resetMixinRegistry()
