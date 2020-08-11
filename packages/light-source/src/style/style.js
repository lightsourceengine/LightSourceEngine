/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Style } from '../addon'

/**
 * Create a Style instance from style properties in a plain old object.
 *
 * @param properties Object containing style properties
 * @returns Style
 */
export const style = properties => {
  if (!properties || typeof properties !== 'object') {
    throw Error(`Invalid style argument: ${properties}`)
  }

  const style = new Style()

  for (const prop in properties) {
    if (prop in Style.prototype) {
      style[prop] = properties[prop]
    }
  }

  return style
}
