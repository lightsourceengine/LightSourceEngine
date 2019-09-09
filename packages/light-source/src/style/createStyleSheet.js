/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Style } from './Style'
import { isObject } from '../util'

/**
 * Create a new style sheet.
 *
 * @param {Object} spec Object containing style names as keys and Style specs as values.
 */
export const createStyleSheet = (spec) => {
  if (!isObject(spec)) {
    throw Error(`Invalid createStyleSheet argument: ${spec}`)
  }

  const result = {}

  for (const key in spec) {
    const entry = spec[key]

    result[key] = entry instanceof Style ? entry : new Style(spec[key])
  }

  return result
}
