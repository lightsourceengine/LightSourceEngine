/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { rgba } from '../../src/style/rgba'

const alphaBit = 0xFFFFFFFF + 1

describe('rgba()', () => {
  it('should return integer in 0x1AARRGGBB color format', () => {
    const values = [
      rgba(0x66, 0x33, 0x99, 0.5),
      rgba('#663399', 0.5),
      rgba('#FF663399', 0.5),
      rgba('#639', 0.5),
      rgba('#F639', 0.5),
      rgba(0x663399, 0.5),
      rgba('rebeccapurple', 0.5),
      rgba('REBECCAPURPLE', 0.5)
    ]

    for (const value of values) {
      assert.equal(value, 0x7F663399 + alphaBit)
    }
  })
  it('should return 0 when argument count does not equal 2 or 4', () => {
    const values = [
      rgba(),
      rgba(0xFF),
      rgba(0xFF, 0xFF, 0xFF),
      rgba(0xFF, 0xFF, 0xFF, 0xFF, 0xFF)
    ]

    for (const value of values) {
      assert.equal(value, 0xFF000000 + alphaBit)
    }
  })
  it('should return 0 when arguments are invalid color components', () => {
    const values = [
      rgba(null, null, null, 1.0),
      rgba(undefined, undefined, undefined, 1.0),
      rgba('', '', '', 1.0),
      rgba([], [], [], 1.0),
      rgba({}, {}, {}, 1.0),
      rgba(NaN, NaN, NaN, 1.0),
      rgba('blue  ', 1.0),
      rgba('fff', 1.0),
      rgba('#ffy', 1.0),
      rgba('#ffyy', 1.0)
    ]

    for (const value of values) {
      assert.equal(value, 0xFF000000 + alphaBit)
    }
  })
})
