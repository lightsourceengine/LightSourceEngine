/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

const kReservedIds = new Set(['@expand'])

export class ShorthandRegistry {
  static add (id, expand) {
    if (typeof id !== 'string' || !id.startsWith('@')) {
      throw Error(`id must be a string that starts with '@' (${id})`)
    }

    if (kReservedIds.has(id)) {
      throw Error(`${id} is a reserved id`)
    }

    if (typeof expand !== 'function') {
      throw Error('expand must be a function')
    }

    if (kShorthandMap.has(id)) {
      throw Error(`Shorthand '${id}' already exists`)
    }

    kShorthandMap.set(id, expand)
  }

  static get (id) {
    return kShorthandMap.get(id)
  }

  static delete (id) {
    kShorthandMap.delete(id)
  }

  static has (id) {
    return kShorthandMap.has(id)
  }
}

const trbl = (s, value) => {
  if (Array.isArray(value)) {
    [s.top, s.right, s.bottom, s.left] = value
  } else {
    s.top = s.right = s.bottom = s.left = value
  }
}

const size = (s, value) => {
  if (Array.isArray(value)) {
    [s.width, s.height] = value
  } else {
    s.width = s.height = value
  }
}

const objectPosition = (s, value) => {
  if (Array.isArray(value)) {
    [s.objectPositionX, s.objectPositionY] = value
  } else {
    s.objectPositionX = s.objectPositionY = value
  }
}

const backgroundPosition = (s, value) => {
  if (Array.isArray(value)) {
    [s.backgroundPositionX, s.backgroundPositionY] = value
  } else {
    s.backgroundPositionX = s.backgroundPositionY = value
  }
}

const transformOrigin = (s, value) => {
  if (Array.isArray(value)) {
    [s.transformOriginX, s.transformOriginY] = value
  } else {
    s.transformOriginX = s.transformOriginY = value
  }
}

const kShorthandMap = new Map()

export const resetShorthandRegistry = () => {
  kShorthandMap.clear()
  kShorthandMap.set('@trbl', trbl)
  kShorthandMap.set('@wh', size)
  kShorthandMap.set('@size', size)
  kShorthandMap.set('@objectPosition', objectPosition)
  kShorthandMap.set('@backgroundPosition', backgroundPosition)
  kShorthandMap.set('@transformOrigin', transformOrigin)
}

resetShorthandRegistry()
