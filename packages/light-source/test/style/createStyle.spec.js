/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { createStyle } from '../../src/style/createStyle'
import { Style, StyleUnit } from '../../src/addon'

describe('createStyle()', () => {
  it('should create a new style sheet', () => {
    const style = createStyle({
      border: 10,
      margin: 15
    })

    assert.instanceOf(style, Style)
    assert.deepInclude(style, { border: [10, StyleUnit.Point], margin: [15, StyleUnit.Point] })
  })
  it('should process shorthand properties', () => {
    const style = createStyle({
      '@size': 50
    })

    assert.instanceOf(style, Style)
    assert.deepInclude(style, { width: [50, StyleUnit.Point], height: [50, StyleUnit.Point] })
  })
  it('should return spec if instanceof Style', () => {
    const style = new Style()

    assert.strictEqual(createStyle(style), style)
  })
  it('should return null for invalid spec', () => {
    for (const input of [null, undefined, '', 3, NaN]) {
      assert.isNull(createStyle(input))
    }
  })
})
