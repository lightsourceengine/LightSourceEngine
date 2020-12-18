/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { waypoint } from '../../src/scene/waypoint.js'

const { assert } = chai

describe('waypoint()', () => {
  it('should return a waypoint when given a "horizontal" tag', () => {
    assert.equal(waypoint('horizontal').tag, 'horizontal')
  })
  it('should return a waypoint when given a "vertical" tag', () => {
    assert.equal(waypoint('vertical').tag, 'vertical')
  })
  it('should throw Error when tag is not "vertical" or "horizontal"', () => {
    for (const input of ['test', '', null, undefined, {}, 3]) {
      assert.throws(() => waypoint(input))
    }
  })
})
