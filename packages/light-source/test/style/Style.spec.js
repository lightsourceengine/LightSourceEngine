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

const testStyleValueEmpty = (name, value) => testStyleValue(name, value, undefined)

const testStyleUnitValue = (name, value, expectedUnit, expectedValue) => {
  assert.deepEqual(style({ [name]: value })[name], { unit: expectedUnit, value: expectedValue })
}

const invalidStringValues = ['', 3, null, undefined, {}]
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
      for (const property of borderProperties) {
        for (const input of ['5%', 'auto', 'left', -1, '-1px']) {
          testStyleValueEmpty(property, input)
        }
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
    it('should return 0 for invalid color values', () => {
      for (const property of colorProperties) {
        testStyleValue(property, '', 0)
      }
    })
  })
  describe('backgroundClip property', () => {
    const property = 'backgroundClip'
    it('should set values', () => {
      for (const value of ['border-box', 'padding-box']) {
        testStyleValue(property, value, value)
      }
    })
    it('should reject invalid values', () => {
      for (const value of invalidStringValues) {
        testStyleValueEmpty(property, value)
      }
    })
  })
  describe('backgroundFit property', () => {
    const property = 'backgroundFit'
    it('should set values', () => {
      for (const value of ['fill', 'contain', 'cover', 'none', 'scale-down']) {
        testStyleValue(property, value, value)
      }
    })
    it('should reject invalid values', () => {
      for (const value of invalidStringValues) {
        testStyleValueEmpty(property, value)
      }
    })
  })
  describe('backgroundWidth property', () => {
    const property = 'backgroundWidth'
    it('should set value', () => {
      testStyleUnitValue(property, 50, Style.UnitPoint, 50)
      testStyleUnitValue(property, '100%', Style.UnitPercent, 100)
      testStyleUnitValue(property, '50px', Style.UnitPoint, 50)
      testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
      testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
      testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
      testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
      testStyleUnitValue(property, '5rem', Style.UnitRootEm, 5)
    })
    it('should reject invalid value', () => {
      for (const input of [-50, '', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('backgroundPositionX property', () => {
    const property = 'backgroundPositionX'
    it('should set value', () => {
      testStyleUnitValue(property, 50, Style.UnitPoint, 50)
      testStyleUnitValue(property, -50, Style.UnitPoint, -50)
      testStyleUnitValue(property, '100%', Style.UnitPercent, 100)
      testStyleUnitValue(property, '50px', Style.UnitPoint, 50)
      testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
      testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
      testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
      testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
      testStyleUnitValue(property, '5rem', Style.UnitRootEm, 5)
      testStyleUnitValue(property, 'left', Style.UnitAnchor, 3)
      testStyleUnitValue(property, 'right', Style.UnitAnchor, 1)
    })
    it('should reject invalid value', () => {
      for (const input of ['', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('backgroundPositionY property', () => {
    const property = 'backgroundPositionY'
    it('should set value', () => {
      testStyleUnitValue(property, 50, Style.UnitPoint, 50)
      testStyleUnitValue(property, -50, Style.UnitPoint, -50)
      testStyleUnitValue(property, '100%', Style.UnitPercent, 100)
      testStyleUnitValue(property, '50px', Style.UnitPoint, 50)
      testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
      testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
      testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
      testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
      testStyleUnitValue(property, '5rem', Style.UnitRootEm, 5)
      testStyleUnitValue(property, 'top', Style.UnitAnchor, 0)
      testStyleUnitValue(property, 'bottom', Style.UnitAnchor, 2)
    })
    it('should reject invalid value', () => {
      for (const input of ['', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('backgroundWidth property', () => {
    const property = 'backgroundWidth'
    it('should set value', () => {
      testStyleUnitValue(property, 50, Style.UnitPoint, 50)
      testStyleUnitValue(property, '100%', Style.UnitPercent, 100)
      testStyleUnitValue(property, '50px', Style.UnitPoint, 50)
      testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
      testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
      testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
      testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
      testStyleUnitValue(property, '5rem', Style.UnitRootEm, 5)
    })
    it('should reject invalid value', () => {
      for (const input of [-50, '', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
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
      for (const property of borderRadiusProperties) {
        for (const input of ['5%', 'auto', 'left', -1, '-1px']) {
          testStyleValueEmpty(property, input)
        }
      }
    })
  })
  describe('fontSize property', () => {
    const property = 'fontSize'
    it('should set values', () => {
      testStyleUnitValue(property, 1, Style.UnitPoint, 1)
      testStyleUnitValue(property, '50px', Style.UnitPoint, 50)
      testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
      testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
      testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
      testStyleUnitValue(property, '5rem', Style.UnitRootEm, 5)
    })
    it('should reject invalid values', () => {
      for (const input of [0, -1, '10', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('fontStyle property', () => {
    const property = 'fontStyle'
    it('should set string value', () => {
      for (const value of ['italic', 'normal']) {
        testStyleValue(property, value, value)
      }
    })
    it('should reject invalid values', () => {
      for (const value of invalidStringValues) {
        testStyleValueEmpty(property, value)
      }
    })
  })
  describe('fontWeight property', () => {
    const property = 'fontWeight'
    it('should set fontWeight string value', () => {
      for (const value of ['bold', 'normal']) {
        testStyleValue(property, value, value)
      }
    })
    it('should reject invalid values', () => {
      for (const value of invalidStringValues) {
        testStyleValueEmpty(property, value)
      }
    })
  })
  describe('lineHeight property', () => {
    const property = 'lineHeight'
    it('should set values', () => {
      testStyleUnitValue(property, 1, Style.UnitPercent, 100)
      testStyleUnitValue(property, '100%', Style.UnitPercent, 100)
      testStyleUnitValue(property, '50px', Style.UnitPoint, 50)
      testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
      testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
      testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
      testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
      testStyleUnitValue(property, '5rem', Style.UnitRootEm, 5)
    })
    it('should reject invalid values', () => {
      for (const input of [-1, null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('maxLines property', () => {
    const property = 'maxLines'
    it('should set values', () => {
      testStyleUnitValue(property, 0, Style.UnitPoint, 0)
      testStyleUnitValue(property, 1, Style.UnitPoint, 1)
    })
    it('should reject invalid values', () => {
      for (const input of [-1, '10', '10px', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('objectFit property', () => {
    const property = 'objectFit'
    it('should set string value', () => {
      for (const value of ['fill', 'contain', 'cover', 'none', 'scale-down']) {
        testStyleValue(property, value, value)
      }
    })
    it('should reject invalid values', () => {
      for (const value of invalidStringValues) {
        testStyleValueEmpty(property, value)
      }
    })
  })
  describe('objectPositionX property', () => {
    const property = 'objectPositionX'
    it('should set value', () => {
      testStyleUnitValue(property, 50, Style.UnitPoint, 50)
      testStyleUnitValue(property, -50, Style.UnitPoint, -50)
      testStyleUnitValue(property, '100%', Style.UnitPercent, 100)
      testStyleUnitValue(property, '50px', Style.UnitPoint, 50)
      testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
      testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
      testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
      testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
      testStyleUnitValue(property, '5rem', Style.UnitRootEm, 5)
      testStyleUnitValue(property, 'left', Style.UnitAnchor, 3)
      testStyleUnitValue(property, 'right', Style.UnitAnchor, 1)
    })
    it('should reject invalid value', () => {
      for (const input of ['', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('objectPositionY property', () => {
    const property = 'objectPositionY'
    it('should set value', () => {
      testStyleUnitValue(property, 50, Style.UnitPoint, 50)
      testStyleUnitValue(property, -50, Style.UnitPoint, -50)
      testStyleUnitValue(property, '100%', Style.UnitPercent, 100)
      testStyleUnitValue(property, '50px', Style.UnitPoint, 50)
      testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
      testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
      testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
      testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
      testStyleUnitValue(property, '5rem', Style.UnitRootEm, 5)
      testStyleUnitValue(property, 'top', Style.UnitAnchor, 0)
      testStyleUnitValue(property, 'bottom', Style.UnitAnchor, 2)
    })
    it('should reject invalid value', () => {
      for (const input of ['', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('opacity property', () => {
    const property = 'opacity'
    it('should set value', () => {
      testStyleUnitValue(property, 0, Style.UnitPoint, 0)
      testStyleUnitValue(property, 1, Style.UnitPoint, 1)
    })
    it('should reject value outside of range [0,1]', () => {
      testStyleValueEmpty(property, -1)
      testStyleValueEmpty(property, 2)
    })
    it('should reject invalid value', () => {
      for (const value of ['', null, undefined, {}, NaN, '30px']) {
        testStyleValueEmpty(property, value)
      }
    })
  })
  describe('textAlign property', () => {
    const property = 'textAlign'
    it('should set string value', () => {
      for (const value of ['left', 'center', 'right']) {
        testStyleValue(property, value, value)
      }
    })
    it('should reject invalid values', () => {
      for (const value of invalidStringValues) {
        testStyleValueEmpty(property, value)
      }
    })
  })
  describe('textOverflow property', () => {
    const property = 'textOverflow'
    it('should set string value', () => {
      for (const value of ['none', 'clip', 'ellipsis']) {
        testStyleValue(property, value, value)
      }
    })
    it('should reject invalid values', () => {
      for (const value of invalidStringValues) {
        testStyleValueEmpty(property, value)
      }
    })
  })
  describe('textTransform property', () => {
    const property = 'textTransform'
    it('should set string value', () => {
      for (const value of ['none', 'lowercase', 'uppercase']) {
        testStyleValue(property, value, value)
      }
    })
    it('should reject invalid values', () => {
      for (const value of invalidStringValues) {
        testStyleValueEmpty(property, value)
      }
    })
  })
})
