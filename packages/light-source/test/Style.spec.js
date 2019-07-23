/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { Style } from '../src/Style'

const style = (obj) => new Style(obj)

const testSetPropertyValue = (name, value, expectedUnit, expectedValue) => {
  assert.deepEqual(style({ [name]: value })[name], { unit: expectedUnit, value: expectedValue })
}

const borderProperties = ['border', 'borderTop', 'borderRight', 'borderBottom', 'borderLeft']

describe('Style', () => {
  describe('border properties', () => {
    it('it should set point and viewport values', () => {
      for (const property of borderProperties) {
        testSetPropertyValue(property, 5, Style.UnitPoint, 5)
        testSetPropertyValue(property, '5px', Style.UnitPoint, 5)
        testSetPropertyValue(property, 0, Style.UnitPoint, 0)
        testSetPropertyValue(property, '0px', Style.UnitPoint, 0)
        testSetPropertyValue(property, '5vw', Style.UnitViewportWidth, 5)
        testSetPropertyValue(property, '5vh', Style.UnitViewportHeight, 5)
        testSetPropertyValue(property, '5vmin', Style.UnitViewportMin, 5)
        testSetPropertyValue(property, '5vmax', Style.UnitViewportMax, 5)
      }
    })
    it('should reject %, auto, anchors and negative values', () => {
      // TODO: native code is throwing a cryptic error
      for (const property of borderProperties) {
        assert.throws(() => style({ [property]: '5%' }))
        assert.throws(() => style({ [property]: 'auto' }))
        assert.throws(() => style({ [property]: 'left' }))
        assert.throws(() => style({ [property]: -1 }))
        assert.throws(() => style({ [property]: '-1px' }))
      }
    })
  })
})
