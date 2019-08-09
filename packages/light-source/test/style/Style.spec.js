/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { Style } from '../../src/style/Style'

const alphaBit = 4294967296

const style = (obj) => new Style(obj)

const testStyleValue = (name, value, expectedValue) => {
  assert.equal(style({ [name]: value })[name], expectedValue)
}

const testStyleUnitValue = (name, value, expectedUnit, expectedValue) => {
  assert.deepEqual(style({ [name]: value })[name], { unit: expectedUnit, value: expectedValue })
}

const borderProperties = ['border', 'borderTop', 'borderRight', 'borderBottom', 'borderLeft']
const borderRadiusProperties = ['borderRadius', 'borderRadiusTopLeft', 'borderRadiusTopRight', 'borderRadiusBottomRight', 'borderRadiusBottomLeft']
const colorProperties = ['color', 'tintColor', 'backgroundColor', 'borderColor']

describe('Style', () => {
  describe('border properties', () => {
    it('should set point and viewport values', () => {
      for (const property of borderProperties) {
        testStyleUnitValue(property, 5, Style.UnitPoint, 5)
        testStyleUnitValue(property, '5px', Style.UnitPoint, 5)
        testStyleUnitValue(property, 0, Style.UnitPoint, 0)
        testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
        testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
        testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
        testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
        testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
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
  describe('borderRadius properties', () => {
    it('should set point and viewport values', () => {
      for (const property of borderRadiusProperties) {
        testStyleUnitValue(property, 5, Style.UnitPoint, 5)
        testStyleUnitValue(property, '5px', Style.UnitPoint, 5)
        testStyleUnitValue(property, 0, Style.UnitPoint, 0)
        testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
        testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
        testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
        testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
        testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
      }
    })
    it('should reject %, auto, anchors and negative values', () => {
      // TODO: native code is throwing a cryptic error
      for (const property of borderRadiusProperties) {
        assert.throws(() => style({ [property]: '5%' }))
        assert.throws(() => style({ [property]: 'auto' }))
        assert.throws(() => style({ [property]: 'left' }))
        assert.throws(() => style({ [property]: -1 }))
        assert.throws(() => style({ [property]: '-1px' }))
      }
    })
  })
  describe('color properties', () => {
    it('should set color values', () => {
      for (const property of colorProperties) {
        testStyleValue(property, 'blanchedalmond', 0xFFEBCD)
        testStyleValue(property, 'BLANCHEDALMOND', 0xFFEBCD)
        testStyleValue(property, 0xFFEBCD, 0xFFEBCD)
        testStyleValue(property, 0xFFFFEBCD, 0xFFEBCD)
        testStyleValue(property, '#FFEBCD', 0xFFEBCD)
        testStyleValue(property, '#ffebcd', 0xFFEBCD)
        testStyleValue(property, '#FFF', 0xFFFFFF)
        testStyleValue(property, '#fff', 0xFFFFFF)
        testStyleValue(property, '#ffffebcd', 0xFFFFEBCD + alphaBit)
        testStyleValue(property, '#ffff', 0xFFFFFFFF + alphaBit)
      }
    })
    it('should reject invalid color values', () => {
      for (const property of colorProperties) {
        assert.throws(() => style({ [property]: '' }))
      }
    })
  })
})
