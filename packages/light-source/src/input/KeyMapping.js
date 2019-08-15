/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

export class KeyMapping {
  constructor (name, entries) {
    if (!name || typeof name !== 'string') {
      throw Error('KeyMapping: name must be a string')
    }

    if (entries && !Array.isArray(entries)) {
      throw Error('KeyMapping: entries must be an array')
    }

    this.name = name
    this.keyMap = new Map()

    for (const entry of entries) {
      this.keyMap.set(entry[1], entry[0])
    }
  }

  getKey (button) {
    return this.keyMap.get(button)
  }

  hasKey (button) {
    return this.keyMap.has(button)
  }
}
