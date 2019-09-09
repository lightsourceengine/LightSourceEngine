/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { createStyleSheet } from '../../src/style/createStyleSheet'
import { rgb } from '../../src/exports'
import { Style } from '../../src/style/Style'

describe('createStyleSheet()', () => {
  it('should return an empty style sheet given an empty object', () => {
    const sheet = createStyleSheet({})

    assert.lengthOf(Object.keys(sheet), 0)
  })
  it('should return a style sheet populated with style objects', () => {
    const sheet = createStyleSheet({
      style1: {
        color: rgb(255, 255, 255)
      },
      style2: {
        left: 10
      }
    })

    assert.lengthOf(Object.keys(sheet), 2)
    assert.instanceOf(sheet.style1, Style)
    assert.instanceOf(sheet.style2, Style)
  })
  it('should throw Error for invalid argument', () => {
    assert.throws(() => createStyleSheet())
    assert.throws(() => createStyleSheet([]))
    assert.throws(() => createStyleSheet(Buffer.from('')))
    assert.throws(() => createStyleSheet(1))
    assert.throws(() => createStyleSheet(''))
  })
})
