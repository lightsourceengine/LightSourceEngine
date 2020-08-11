/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Style } from '../addon'
import { style } from './style'

/**
 * Create a new style sheet.
 *
 * @param {Object} spec Object containing style names as keys and Style specs as values.
 */
export const createStyleSheet = (spec) => {
  if (!spec || typeof spec !== 'object') {
    throw Error(`Invalid createStyleSheet argument: ${spec}`)
  }

  const result = {}

  for (const key in spec) {
    const entry = spec[key]

    if (entry instanceof Style) {
      result[key] = entry
    } else {
      result[key] = style(entry)
    }
  }

  return result
}
