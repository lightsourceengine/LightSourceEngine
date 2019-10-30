/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { rgb } from '../../src/style/rgb'

describe('rgb()', () => {
  it('should return integer in 0xRRGGBB color format', () => {
    assert.equal(rgb(0x66, 0x33, 0x99), 0xFF663399)
  })
  it('should clamp out of range color component values', () => {
    assert.equal(rgb(256, 300, 1000), 0xFFFFFFFF)
    assert.equal(rgb(-1, -200, 0xFF), 0xFF0000FF)
  })
  it('should return opaque black when arguments are non-number color components', () => {
    assert.equal(rgb(NaN, NaN, NaN), 0xFF000000)
  })
})
