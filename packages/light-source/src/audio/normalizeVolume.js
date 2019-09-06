/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

export const normalizeVolume = value => {
  if (typeof value === 'number') {
    if (value < 0) {
      value = 0
    } else if (value > 1) {
      value = 1
    }

    return value
  }

  return undefined
}
