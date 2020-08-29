/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Style } from '../addon'
import { ShorthandRegistry } from './ShorthandRegistry'
import { emptyObject } from '../util'
import { MixinRegistry } from './MixinRegistry'

/**
 * Create a Style instance from plain Object of style properties.
 *
 * All compatible Style properties and values are supported. If the properties object contains a non-Style property,
 * the property is ignored.
 *
 * Shorthand notation is supported. For example, { '&#64;size': 0 } will be expanded to { width: 0, height: 0 } in the
 * final Style object.
 *
 * Mixins are supported with '&#64;extend', but the ids can only reference mixins in the MixinRegistry.
 *
 * The returned Style object must be treated as an immutable Style "class" object. SceneNodes have Style "instance"
 * objects, bound to the SceneNode. The "class" can be applied to an "instance", which will simply assign properties
 * to the instance.
 *
 * @param properties {Object} Plain Object containing style properties and values
 * @returns {Style} Immutable Style class object
 */
export const createStyle = (properties) => {
  if (properties instanceof Style) {
    return properties
  }
  return createStyleInternal(properties, '', emptyObject)
}

/**
 * @ignore
 */
export const createStyleInStyleSheet = (style, key, styleSheet) => {
  if (style instanceof Style) {
    throw Error('Style objects not allowed in style sheet creation spec')
  }
  return createStyleInternal(style, key, styleSheet)
}

const createStyleInternal = (style, key, styleSheet) => {
  if (!style || typeof style !== 'object') {
    return null
  }

  if (kExtendKey in style) {
    style = processExtend(style, key, styleSheet)
  }

  const instance = new Style()

  for (const prop in style) {
    if (prop in Style.prototype) {
      instance[prop] = style[prop]
    } else if (ShorthandRegistry.has(prop)) {
      ShorthandRegistry.get(prop)(instance, style[prop])
    }
    // ignore unknown property
  }

  // TODO: need to make style immutable at the native layer
  return instance
}

const processExtend = (style, key, styleSheet) => {
  if (hasExtendCycle(style, key, styleSheet)) {
    throw Error('circular reference detected in @extend chain')
  }

  return extend(style, styleSheet)
}

/**
 * Flattens styles and mixins references by @extend into one style property spec.
 * @ignore
 */
const extend = (style, styleSheet) => {
  if (kExtendKey in style) {
    const extendValue = style[kExtendKey]

    if (Array.isArray(extendValue)) {
      const parent = {}

      for (const extendEntry of extendValue) {
        Object.assign(parent, extendSingle(extendEntry, styleSheet))
      }

      return Object.assign({}, parent, style)
    } else {
      return Object.assign({}, extendSingle(extendValue, styleSheet), style)
    }
  }

  return style
}

const extendSingle = (extendValue, styleSheet) => {
  if (typeof extendValue === 'string') {
    if (MixinRegistry.has(extendValue)) {
      return extend(MixinRegistry.get(extendValue), null)
    } else {
      if (!styleSheet || !Object.prototype.hasOwnProperty.call(styleSheet, extendValue)) {
        throw Error(`unreferenced extend class: ${extendValue}`)
      }

      return extend(styleSheet[extendValue], styleSheet)
    }
  } else if (extendValue instanceof Style) {
    throw Error('cannot extend a Style, use a plain Object')
  } else if (typeof extendValue === 'object') {
    return extendValue
  }

  throw Error('unsupported extend value')
}

const isLeaf = (style) => (!(kExtendKey in style))

const findStyle = (extendValue, styleSheet) => {
  const style = styleSheet?.[extendValue] ?? MixinRegistry.get(extendValue)

  if (!style) {
    Error(`unreferenced extend value: ${extendValue}`)
  }

  return style
}

/**
 * Checks for circular references in the @extend chain.
 * @ignore
 */
const hasExtendCycle = (style, styleId, styleSheet) => {
  if (isLeaf(style)) {
    return
  }

  const stackSet = new Set()
  let detected = false

  const detect = (style, styleId) => {
    if (isLeaf(style)) {
      return
    }

    if (stackSet.has(styleId)) {
      detected = true
      return
    }

    stackSet.add(styleId)

    const extendValue = style[kExtendKey]

    if (Array.isArray(extendValue)) {
      for (const value of extendValue) {
        detect(findStyle(value, styleSheet), value)
      }
    } else if (extendValue && typeof extendValue === 'string') {
      detect(findStyle(extendValue, styleSheet), extendValue)
    } else {
      throw Error(`unknown @extend value: ${extendValue}`)
    }

    stackSet.delete(styleId)
  }

  detect(style, styleId)

  return detected
}

const kExtendKey = '@extend'