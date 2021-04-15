/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import chai from 'chai'
import { StyleAnchor, StyleUnit } from '../../src/addon/index.js'
import { rotate } from '../../src/style/transform.js'
import { fileuri } from '../../src/util/index.js'
import { StyleInstance } from '../../src/style/StyleInstance.js'
import { StyleClass } from '../../src/style/StyleClass.js'
import { StyleValue } from '../../src/style/StyleValue.js'
import { flipH, flipV, tint } from '../../src/style/filter.js'

const { assert } = chai

const style = (obj) => Object.assign(new StyleInstance(), obj)

const testStyleValue = (name, value, expectedValue) => {
  assert.deepEqual(style({ [name]: value })[name], expectedValue)
}

const testStyleValueUndefined = (name, value) => {
  assert.isUndefined(style({ [name]: value })[name])
}

const testStyleValueEmpty = (name, value) => {
  assert.isTrue(style({ [name]: value })[name].isUndefined())
}

const testStyleUnitValue = (name, value, expectedUnit, expectedValue) => {
  assert.deepEqual(style({ [name]: value })[name], new StyleValue(expectedValue, expectedUnit))
}

const testIntegerValue = (name, value, expectedValue) => {
  assert.equal(style({ [name]: value })[name], expectedValue)
}

const invalidStringValues = ['', 3, null, undefined, {}]
const borderProperties = ['border', 'borderTop', 'borderRight', 'borderBottom', 'borderLeft']
const colorProperties = ['color', 'backgroundColor', 'borderColor']

describe('Style', () => {
  describe('border properties', () => {
    it('should set point and viewport values', () => {
      for (const property of borderProperties) {
        testStyleUnitValue(property, 5, StyleUnit.POINT, 5)
        testStyleUnitValue(property, '5px', StyleUnit.POINT, 5)
        testStyleUnitValue(property, 0, StyleUnit.POINT, 0)
        testStyleUnitValue(property, '0px', StyleUnit.POINT, 0)
        testStyleUnitValue(property, '5vw', StyleUnit.VIEWPORT_WIDTH, 5)
        testStyleUnitValue(property, '5vh', StyleUnit.VIEWPORT_HEIGHT, 5)
        testStyleUnitValue(property, '5vmin', StyleUnit.VIEWPORT_MIN, 5)
        testStyleUnitValue(property, '5vmax', StyleUnit.VIEWPORT_MAX, 5)
      }
    })
    it('should undefined StyleValue for invalid %, auto, anchors and negative values', () => {
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
        testStyleValue(property, 'blanchedalmond', 0xFFFFEBCD)
        testStyleValue(property, 'BLANCHEDALMOND', 0xFFFFEBCD)
        testStyleValue(property, 0xFFEBCD, 0xFFEBCD)
        testStyleValue(property, 0xFFFFEBCD, 0xFFFFEBCD)
        testStyleValue(property, '#FFEBCD', 0xFFFFEBCD)
        testStyleValue(property, '#ffebcd', 0xFFFFEBCD)
        testStyleValue(property, '#FFF', 0xFFFFFFFF)
        testStyleValue(property, '#fff', 0xFFFFFFFF)
        testStyleValue(property, '#ffebcdff', 0xFFFFEBCD)
        testStyleValue(property, '#ffff', 0xFFFFFFFF)
      }
    })
    it('should return undefined for invalid color values', () => {
      for (const property of colorProperties) {
        testStyleValueUndefined(property, '')
      }
    })
  })
  describe('backgroundClip property', () => {
    const property = 'backgroundClip'
    const enums = ['border-box', 'padding-box']
    it('should set values', () => {
      testEnumProperty(property, enums)
    })
    it('should use default enum value when given invalid input', () => {
      testInvalidEnumProperty(property, enums[0])
    })
  })
  describe('backgroundSize property', () => {
    const property = 'backgroundSize'
    const enums = ['none', 'contain', 'cover']
    it('should set values', () => {
      testEnumProperty(property, enums)
    })
    it('should use default enum value when given invalid input', () => {
      testInvalidEnumProperty(property, enums[0])
    })
  })
  describe('backgroundRepeat property', () => {
    const property = 'backgroundRepeat'
    const enums = ['repeat', 'repeat-x', 'repeat-y', 'no-repeat']
    it('should set values', () => {
      testEnumProperty(property, enums)
    })
    it('should use default enum value when given invalid input', () => {
      testInvalidEnumProperty(property, 'no-repeat')
    })
  })
  describe('backgroundWidth property', () => {
    const property = 'backgroundWidth'
    it('should set value', () => {
      testDimensionProperty(property)
    })
    it('should reject invalid value', () => {
      testInvalidDimensionProperty(property)
    })
  })
  describe('backgroundHeight property', () => {
    const property = 'backgroundHeight'
    it('should set value', () => {
      testDimensionProperty(property)
    })
    it('should reject invalid value', () => {
      testInvalidDimensionProperty(property)
    })
  })
  describe('backgroundPositionX property', () => {
    const property = 'backgroundPositionX'
    it('should set value', () => {
      testPositionProperty(property, xDirection)
    })
    it('should reject invalid value', () => {
      testInvalidPositionProperty(property, xDirection)
    })
  })
  describe('backgroundPositionY property', () => {
    const property = 'backgroundPositionY'
    it('should set value', () => {
      testPositionProperty(property, yDirection)
    })
    it('should reject invalid value', () => {
      testInvalidPositionProperty(property, yDirection)
    })
  })
  describe('fontSize property', () => {
    const property = 'fontSize'
    it('should set values', () => {
      testStyleUnitValue(property, 1, StyleUnit.POINT, 1)
      testStyleUnitValue(property, '50px', StyleUnit.POINT, 50)
      testStyleUnitValue(property, '5vw', StyleUnit.VIEWPORT_WIDTH, 5)
      testStyleUnitValue(property, '5vh', StyleUnit.VIEWPORT_HEIGHT, 5)
      testStyleUnitValue(property, '5vmin', StyleUnit.VIEWPORT_MIN, 5)
      testStyleUnitValue(property, '5vmax', StyleUnit.VIEWPORT_MAX, 5)
      testStyleUnitValue(property, '5rem', StyleUnit.REM, 5)
    })
    it('should reject invalid values', () => {
      for (const input of [-1, '10', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('fontStyle property', () => {
    const property = 'fontStyle'
    const enums = ['normal', 'italic', 'oblique']
    it('should set values', () => {
      testEnumProperty(property, enums)
    })
    it('should use default enum value when given invalid input', () => {
      testInvalidEnumProperty(property, enums[0])
    })
  })
  describe('fontWeight property', () => {
    const property = 'fontWeight'
    const enums = ['normal', 'bold']
    it('should set values', () => {
      testEnumProperty(property, enums)
    })
    it('should use default enum value when given invalid input', () => {
      testInvalidEnumProperty(property, enums[0])
    })
  })
  describe('lineHeight property', () => {
    const property = 'lineHeight'
    it('should set values', () => {
      testStyleUnitValue(property, 100, StyleUnit.POINT, 100)
      testStyleUnitValue(property, '100%', StyleUnit.PERCENT, 100)
      testStyleUnitValue(property, '50px', StyleUnit.POINT, 50)
      testStyleUnitValue(property, '0px', StyleUnit.POINT, 0)
      testStyleUnitValue(property, '5vw', StyleUnit.VIEWPORT_WIDTH, 5)
      testStyleUnitValue(property, '5vh', StyleUnit.VIEWPORT_HEIGHT, 5)
      testStyleUnitValue(property, '5vmin', StyleUnit.VIEWPORT_MIN, 5)
      testStyleUnitValue(property, '5vmax', StyleUnit.VIEWPORT_MAX, 5)
      testStyleUnitValue(property, '5rem', StyleUnit.REM, 5)
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
      testIntegerValue(property, 0, 0)
      testIntegerValue(property, 1, 1)
      // TODO: NaN gets converted to 0 somewhere..
      testIntegerValue(property, NaN, 0)
    })
    it('should reject invalid values', () => {
      for (const input of [-1, '10', null, {}, [], undefined]) {
        testStyleValueUndefined(property, input)
      }
    })
  })
  describe('objectFit property', () => {
    const property = 'objectFit'
    const enums = ['fill', 'contain', 'cover', 'none', 'scale-down']
    it('should set values', () => {
      testEnumProperty(property, enums)
    })
    it('should use default enum value when given invalid input', () => {
      testInvalidEnumProperty(property, enums[0])
    })
  })
  describe('objectPositionX property', () => {
    const property = 'objectPositionX'
    it('should set value', () => {
      testPositionProperty(property, xDirection)
    })
    it('should reject invalid value', () => {
      testInvalidPositionProperty(property, xDirection)
    })
  })
  describe('objectPositionY property', () => {
    const property = 'objectPositionY'
    it('should set value', () => {
      testPositionProperty(property, yDirection)
    })
    it('should reject invalid value', () => {
      testInvalidPositionProperty(property, yDirection)
    })
  })
  describe('opacity property', () => {
    const property = 'opacity'
    it('should set value in range [0,1]', () => {
      for (const input of [0, 0.5, 1]) {
        testStyleUnitValue(property, input, StyleUnit.POINT, input)
      }
    })
    it('should set percent value in range [0,100]', () => {
      testStyleUnitValue(property, '0%', StyleUnit.PERCENT, 0)
      testStyleUnitValue(property, '50%', StyleUnit.PERCENT, 50)
      testStyleUnitValue(property, '100%', StyleUnit.PERCENT, 100)
    })
    it('should reject value outside of range [0,1]', () => {
      testStyleValueEmpty(property, -1)
      testStyleValueEmpty(property, 2)
    })
    it('should reject percent value outside of range [0,100]', () => {
      testStyleValueEmpty(property, '-1%')
      testStyleValueEmpty(property, '101%')
    })
    it('should reject invalid value', () => {
      for (const value of ['', null, undefined, {}, '30deg']) {
        testStyleValueEmpty(property, value)
      }
    })
  })
  describe('textAlign property', () => {
    const property = 'textAlign'
    const enums = ['left', 'center', 'right']
    it('should set values', () => {
      testEnumProperty(property, enums)
    })
    it('should use default enum value when given invalid input', () => {
      testInvalidEnumProperty(property, enums[0])
    })
  })
  describe('textOverflow property', () => {
    const property = 'textOverflow'
    const enums = ['none', 'clip', 'ellipsis']
    it('should set values', () => {
      testEnumProperty(property, enums)
    })
    it('should use default enum value when given invalid input', () => {
      testInvalidEnumProperty(property, enums[0])
    })
  })
  describe('textTransform property', () => {
    const property = 'textTransform'
    const enums = ['none', 'lowercase', 'uppercase']
    it('should set values', () => {
      testEnumProperty(property, enums)
    })
    it('should use default enum value when given invalid input', () => {
      testInvalidEnumProperty(property, enums[0])
    })
  })
  describe('transform property', () => {
    it('should be initialized to zero length array', () => {
      const styleClass = new StyleClass()
      assert.lengthOf(styleClass.transform, 0)
    })
    it('should be assignable', () => {
      const s1 = style({ transform: [rotate('90deg')] })
      const s2 = style({})

      s2.tranform = s1.transform

      assert.lengthOf(s2.tranform, 1)
      assert.isTrue(s2.tranform[0].isRotate())
    })
    it('should assignable to transform array', () => {
      const s = style({ transform: [rotate('90deg')] })

      assert.lengthOf(s.transform, 1)

      const transformValue = s.transform[0]

      assert.isTrue(transformValue.isRotate())
      assert.deepEqual(transformValue.angle, new StyleValue(90, StyleUnit.DEGREE))
    })
    it('should assignable to transform value', () => {
      const s = style({ transform: rotate('90deg') })

      assert.lengthOf(s.transform, 1)

      const transformValue = s.transform[0]

      assert.isTrue(transformValue.isRotate())
      assert.deepEqual(transformValue.angle, new StyleValue(90, StyleUnit.DEGREE))
    })
    it('should reject invalid values', () => {
      const s = style({ transform: 'invalid' })
      assert.lengthOf(s.transform, 0)
    })
  })
  describe('transformOriginX property', () => {
    const property = 'transformOriginX'
    it('should set value', () => {
      testPositionProperty(property, xDirection)
    })
    it('should reject invalid value', () => {
      testInvalidPositionProperty(property, xDirection)
    })
  })
  describe('transformOriginY property', () => {
    const property = 'transformOriginY'
    it('should set value', () => {
      testPositionProperty(property, yDirection)
    })
    it('should reject invalid value', () => {
      testInvalidPositionProperty(property, yDirection)
    })
  })
  describe('filter property', () => {
    it('should be initialized to zero length array', () => {
      const styleClass = new StyleClass()
      assert.lengthOf(styleClass.filter, 0)
    })
    it('should set filter from flipH()', () => {
      const styleClass = style({ filter: flipH() })
      assert.lengthOf(styleClass.filter, 1)
      assert.deepEqual(styleClass.filter[0], flipH())
    })
    it('should set filter from flipV()', () => {
      const styleClass = style({ filter: flipV() })
      assert.lengthOf(styleClass.filter, 1)
      assert.deepEqual(styleClass.filter[0], flipV())
    })
    it('should set filter from tint()', () => {
      const styleClass = style({ filter: tint('red') })
      assert.lengthOf(styleClass.filter, 1)
      assert.deepEqual(styleClass.filter[0], tint('red'))
    })
    it('should set filter from list', () => {
      const styleClass = style({ filter: [flipH(), flipV(), tint('red')] })
      assert.lengthOf(styleClass.filter, 3)
      assert.deepEqual(styleClass.filter[0], flipH())
      assert.deepEqual(styleClass.filter[1], flipV())
      assert.deepEqual(styleClass.filter[2], tint('red'))
    })
    it('should reject invalid filter value', () => {
      const styleClass = style({ filter: 'invalid' })
      assert.lengthOf(styleClass.filter, 0)
    })
    it('should reject array of invalid filters', () => {
      const styleClass = style({ filter: ['invalid'] })
      assert.lengthOf(styleClass.filter, 0)
    })
    it('should reject array of valid and invalid filters', () => {
      const styleClass = style({ filter: [flipH(), 'invalid'] })
      assert.lengthOf(styleClass.filter, 0)
    })
  })
  describe('backgroundImage property', () => {
    const property = 'backgroundImage'
    const testUri = 'test.jpg'
    it('should be assignable to a string uri', () => {
      testStyleValue(property, testUri, testUri)
    })
    it('should be assignable to an empty string', () => {
      testStyleValue(property, '', '')
    })
    it('should set from uri property and use width & height', () => {
      testStyleValue(property, fileuri(testUri, { width: 10, height: 20 }),
        `file:${testUri}?width=10&height=20`)
    })
    xit('should  set from uri property and use int capInsets', () => {
      testStyleValue(property, { uri: testUri, capInsets: 10 },
        { id: testUri, uri: testUri, capInsets: { top: 10, right: 10, bottom: 10, left: 10 } })
    })
    xit('should  set from uri property and use full capInsets', () => {
      const capInsets = { top: 1, right: 2, bottom: 3, left: 4 }
      testStyleValue(property, { uri: testUri, capInsets }, { id: testUri, uri: testUri, capInsets })
    })
    it('should reject invalid value', () => {
      const inputs = [
        null,
        undefined,
        3,
        NaN,
        {},
        { id: 'x' },
        { uri: 3 }
      ]

      inputs.forEach(input => testStyleValueUndefined(property, input))
    })
  })
  describe('zIndex property', () => {
    const property = 'zIndex'
    it('should set positive value', () => {
      testIntegerValue(property, 1, 1)
      // TODO: NaN is getting set to 0 somewhere
      testIntegerValue(property, NaN, 0)
    })
    it('should set negative value', () => {
      testIntegerValue(property, -1, -1)
    })
    it('should reject invalid value', () => {
      const inputs = [
        '',
        '3',
        null,
        undefined,
        {}
      ]

      inputs.forEach(i => testStyleValueUndefined(property, i))
    })
  })
  describe('assignment', () => {
    let style
    beforeEach(() => {
      style = new StyleInstance()
    })
    afterEach(() => {
      style = null
    })
    it('should be assignable by plain object with value and unit', () => {
      style.width = { value: 100, unit: StyleUnit.POINT }
      assert.deepEqual(style.width, StyleValue.of(100))
    })
    it('should be undefined if plain object has invalid unit', () => {
      style.width = { value: 100, unit: -1 }
      assert.isTrue(style.width.isUndefined())
    })
    it('should be undefined if plain object has invalid value', () => {
      style.width = { unit: StyleUnit.POINT }
      assert.isTrue(style.width.isUndefined())
    })
    it('should be assignable by array of value and unit', () => {
      style.width = [100, StyleUnit.POINT]
      assert.deepEqual(style.width, StyleValue.of(100))
    })
    it('should be undefined if array has invalid unit', () => {
      style.width = [100, -1]
      assert.isTrue(style.width.isUndefined())
    })
    it('should be undefined if array has invalid value', () => {
      style.width = [{}, StyleUnit.POINT]
      assert.isTrue(style.width.isUndefined())
    })
    it('should be assignable by StyleValue', () => {
      style.width = StyleValue.of(100)
      assert.deepEqual(style.width, StyleValue.of(100))
    })
    it('should be assignable by other property', () => {
      style.width = 100
      style.height = style.width
      assert.deepEqual(style.height, StyleValue.of(100))
    })
    it('should be undefined for non-StyleValue values', () => {
      for (const input of [null, undefined, '', 'test', {}, [], NaN]) {
        style.width = StyleValue.of(input)
        assert.isTrue(style.width.isUndefined())
      }
    })
  })
})

const xDirection = 1
const yDirection = 2

const testPositionProperty = (property, direction) => {
  testStyleUnitValue(property, 50, StyleUnit.POINT, 50)
  testStyleUnitValue(property, -50, StyleUnit.POINT, -50)
  testStyleUnitValue(property, '100%', StyleUnit.PERCENT, 100)
  testStyleUnitValue(property, '50px', StyleUnit.POINT, 50)
  testStyleUnitValue(property, '0px', StyleUnit.POINT, 0)
  testStyleUnitValue(property, '5vw', StyleUnit.VIEWPORT_WIDTH, 5)
  testStyleUnitValue(property, '5vh', StyleUnit.VIEWPORT_HEIGHT, 5)
  testStyleUnitValue(property, '5vmin', StyleUnit.VIEWPORT_MIN, 5)
  testStyleUnitValue(property, '5vmax', StyleUnit.VIEWPORT_MAX, 5)
  testStyleUnitValue(property, '5rem', StyleUnit.REM, 5)

  testStyleUnitValue(property, 'center', StyleUnit.ANCHOR, StyleAnchor.CENTER)

  if (direction === xDirection) {
    testStyleUnitValue(property, 'left', StyleUnit.ANCHOR, StyleAnchor.LEFT)
    testStyleUnitValue(property, 'right', StyleUnit.ANCHOR, StyleAnchor.RIGHT)
  } else if (direction === yDirection) {
    testStyleUnitValue(property, 'top', StyleUnit.ANCHOR, StyleAnchor.TOP)
    testStyleUnitValue(property, 'bottom', StyleUnit.ANCHOR, StyleAnchor.BOTTOM)
  } else {
    assert.fail()
  }
}

const testInvalidPositionProperty = (property, direction) => {
  for (const input of ['', null, {}, [], undefined, NaN]) {
    testStyleValueEmpty(property, input)
  }

  if (direction === xDirection) {
    testStyleValueEmpty(property, 'top')
    testStyleValueEmpty(property, 'bottom')
  } else if (direction === yDirection) {
    testStyleValueEmpty(property, 'left')
    testStyleValueEmpty(property, 'right')
  } else {
    assert.fail()
  }
}

const testDimensionProperty = (property) => {
  testStyleUnitValue(property, 50, StyleUnit.POINT, 50)
  testStyleUnitValue(property, '100%', StyleUnit.PERCENT, 100)
  testStyleUnitValue(property, '50px', StyleUnit.POINT, 50)
  testStyleUnitValue(property, '0px', StyleUnit.POINT, 0)
  testStyleUnitValue(property, '5vw', StyleUnit.VIEWPORT_WIDTH, 5)
  testStyleUnitValue(property, '5vh', StyleUnit.VIEWPORT_HEIGHT, 5)
  testStyleUnitValue(property, '5vmin', StyleUnit.VIEWPORT_MIN, 5)
  testStyleUnitValue(property, '5vmax', StyleUnit.VIEWPORT_MAX, 5)
  testStyleUnitValue(property, '5rem', StyleUnit.REM, 5)
}

const testInvalidDimensionProperty = (property) => {
  for (const input of [-50, '', null, {}, [], undefined, NaN]) {
    testStyleValueEmpty(property, input)
  }
}

const testEnumProperty = (property, enums) => {
  for (const value of enums) {
    testStyleValue(property, value, value)
  }
}

const testInvalidEnumProperty = (property, fallback) => {
  assert.equal(style({})[property], fallback)

  for (const value of invalidStringValues) {
    testStyleValue(property, value, fallback)
  }
}
