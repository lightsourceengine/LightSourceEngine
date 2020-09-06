/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { createStyleInStyleSheet } from './createStyle.js'

/**
 * Create a new style sheet.
 *
 * Each style spec is converted to a Style using createStyle(). The same rules apply.
 *
 * When creating a style sheet, local mixins are supported. If a spec key starts with '%', it is treated as a local
 * mixin. A Style will not be generated or returned, but the mixin can be referenced by other specs in this style
 * sheet.
 *
 * @param {Object} spec Object containing style names as keys and style property specs as values. specs are just
 * plain Objects with style properties
 */
export const createStyleSheet = (spec) => {
  if (!spec || typeof spec !== 'object') {
    throw Error(`Invalid createStyleSheet argument: ${spec}`)
  }

  const result = {}

  for (const key in spec) {
    // skip mixins
    if (key.startsWith('%')) {
      continue
    }

    // name that start with @ are reserved by createStyleSheet
    if (key.startsWith('@')) {
      throw Error('style name cannot be prefixed with @')
    }

    const entry = spec[key]

    if (entry) {
      // Object -> Style (createStyleInStyleSheet will handle @extend)
      result[key] = createStyleInStyleSheet(entry, key, spec)
    } else {
      throw Error(`invalid style sheet entry at ${key}`)
    }
  }

  return result
}
