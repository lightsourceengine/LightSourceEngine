/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { Style, StyleAnchor, StyleClass, StyleUnit, StyleValue } from '../../src/addon/index.js'
import { getRotateAngle, isRotate, rotate } from '../../src/style/transform.js'

const { assert } = chai

const style = (obj) => Object.assign(new Style(), obj)

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
const borderRadiusProperties = ['borderRadius', 'borderRadiusTopLeft', 'borderRadiusTopRight', 'borderRadiusBottomRight', 'borderRadiusBottomLeft']
const colorProperties = ['color', 'tintColor', 'backgroundColor', 'borderColor']

describe('Style', () => {
  describe('border properties', () => {
    it('should set point and viewport values', () => {
      for (const property of borderProperties) {
        testStyleUnitValue(property, 5, StyleUnit.Point, 5)
        testStyleUnitValue(property, '5px', StyleUnit.Point, 5)
        testStyleUnitValue(property, 0, StyleUnit.Point, 0)
        testStyleUnitValue(property, '0px', StyleUnit.Point, 0)
        testStyleUnitValue(property, '5vw', StyleUnit.ViewportWidth, 5)
        testStyleUnitValue(property, '5vh', StyleUnit.ViewportHeight, 5)
        testStyleUnitValue(property, '5vmin', StyleUnit.ViewportMin, 5)
        testStyleUnitValue(property, '5vmax', StyleUnit.ViewportMax, 5)
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
  describe('borderRadius properties', () => {
    it('should set point and viewport values', () => {
      for (const property of borderRadiusProperties) {
        testStyleUnitValue(property, 5, StyleUnit.Point, 5)
        testStyleUnitValue(property, '5px', StyleUnit.Point, 5)
        testStyleUnitValue(property, 0, StyleUnit.Point, 0)
        testStyleUnitValue(property, '0px', StyleUnit.Point, 0)
        testStyleUnitValue(property, '5vw', StyleUnit.ViewportWidth, 5)
        testStyleUnitValue(property, '5vh', StyleUnit.ViewportHeight, 5)
        testStyleUnitValue(property, '5vmin', StyleUnit.ViewportMin, 5)
        testStyleUnitValue(property, '5vmax', StyleUnit.ViewportMax, 5)
        testStyleUnitValue(property, '5%', StyleUnit.Percent, 5)
      }
    })
    it('should reject %, auto, anchors and negative values', () => {
      for (const property of borderRadiusProperties) {
        for (const input of ['auto', 'left', -1, '-1px']) {
          testStyleValueEmpty(property, input)
        }
      }
    })
  })
  describe('fontSize property', () => {
    const property = 'fontSize'
    it('should set values', () => {
      testStyleUnitValue(property, 1, StyleUnit.Point, 1)
      testStyleUnitValue(property, '50px', StyleUnit.Point, 50)
      testStyleUnitValue(property, '5vw', StyleUnit.ViewportWidth, 5)
      testStyleUnitValue(property, '5vh', StyleUnit.ViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', StyleUnit.ViewportMin, 5)
      testStyleUnitValue(property, '5vmax', StyleUnit.ViewportMax, 5)
      testStyleUnitValue(property, '5rem', StyleUnit.RootEm, 5)
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
      testStyleUnitValue(property, 100, StyleUnit.Point, 100)
      testStyleUnitValue(property, '100%', StyleUnit.Percent, 100)
      testStyleUnitValue(property, '50px', StyleUnit.Point, 50)
      testStyleUnitValue(property, '0px', StyleUnit.Point, 0)
      testStyleUnitValue(property, '5vw', StyleUnit.ViewportWidth, 5)
      testStyleUnitValue(property, '5vh', StyleUnit.ViewportHeight, 5)
      testStyleUnitValue(property, '5vmin', StyleUnit.ViewportMin, 5)
      testStyleUnitValue(property, '5vmax', StyleUnit.ViewportMax, 5)
      testStyleUnitValue(property, '5rem', StyleUnit.RootEm, 5)
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
        testStyleUnitValue(property, input, StyleUnit.Point, input)
      }
    })
    it('should set percent value in range [0,100]', () => {
      testStyleUnitValue(property, '0%', StyleUnit.Percent, 0)
      testStyleUnitValue(property, '50%', StyleUnit.Percent, 50)
      testStyleUnitValue(property, '100%', StyleUnit.Percent, 100)
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
      assert.isTrue(isRotate(s2.tranform[0]))
    })
    it('should assignable to transform array', () => {
      const s = style({ transform: [rotate('90deg')] })

      assert.lengthOf(s.transform, 1)

      const transformValue = s.transform[0]

      assert.isTrue(isRotate(transformValue))
      assert.deepEqual(getRotateAngle(transformValue), new StyleValue(90, StyleUnit.Degree))
    })
    it('should assignable to transform value', () => {
      const s = style({ transform: rotate('90deg') })

      assert.lengthOf(s.transform, 1)

      const transformValue = s.transform[0]

      assert.isTrue(isRotate(transformValue))
      assert.deepEqual(getRotateAngle(transformValue), new StyleValue(90, StyleUnit.Degree))
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
  describe('backgroundImage property', () => {
    const property = 'backgroundImage'
    const testUri = 'test.jpg'
    it('should be assignable to a string uri', () => {
      testStyleValue(property, testUri, testUri)
    })
    it('should be assignable to an empty string', () => {
      testStyleValue(property, '', '')
    })
    // TODO: support file uri with parameters
    xit('should set from uri property', () => {
      testStyleValue(property, { uri: testUri }, { id: testUri, uri: testUri })
    })
    xit('should set from uri property and reject invalid id', () => {
      testStyleValue(property, { id: 3, uri: testUri }, { id: testUri, uri: testUri })
    })
    xit('should set from uri property and use width & height', () => {
      testStyleValue(property, { uri: testUri, width: 10, height: 20 },
        { id: testUri, uri: testUri, width: 10, height: 20 })
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
})

const xDirection = 1
const yDirection = 2

const testPositionProperty = (property, direction) => {
  testStyleUnitValue(property, 50, StyleUnit.Point, 50)
  testStyleUnitValue(property, -50, StyleUnit.Point, -50)
  testStyleUnitValue(property, '100%', StyleUnit.Percent, 100)
  testStyleUnitValue(property, '50px', StyleUnit.Point, 50)
  testStyleUnitValue(property, '0px', StyleUnit.Point, 0)
  testStyleUnitValue(property, '5vw', StyleUnit.ViewportWidth, 5)
  testStyleUnitValue(property, '5vh', StyleUnit.ViewportHeight, 5)
  testStyleUnitValue(property, '5vmin', StyleUnit.ViewportMin, 5)
  testStyleUnitValue(property, '5vmax', StyleUnit.ViewportMax, 5)
  testStyleUnitValue(property, '5rem', StyleUnit.RootEm, 5)

  testStyleUnitValue(property, 'center', StyleUnit.Anchor, StyleAnchor.Center)

  if (direction === xDirection) {
    testStyleUnitValue(property, 'left', StyleUnit.Anchor, StyleAnchor.Left)
    testStyleUnitValue(property, 'right', StyleUnit.Anchor, StyleAnchor.Right)
  } else if (direction === yDirection) {
    testStyleUnitValue(property, 'top', StyleUnit.Anchor, StyleAnchor.Top)
    testStyleUnitValue(property, 'bottom', StyleUnit.Anchor, StyleAnchor.Bottom)
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
  testStyleUnitValue(property, 50, StyleUnit.Point, 50)
  testStyleUnitValue(property, '100%', StyleUnit.Percent, 100)
  testStyleUnitValue(property, '50px', StyleUnit.Point, 50)
  testStyleUnitValue(property, '0px', StyleUnit.Point, 0)
  testStyleUnitValue(property, '5vw', StyleUnit.ViewportWidth, 5)
  testStyleUnitValue(property, '5vh', StyleUnit.ViewportHeight, 5)
  testStyleUnitValue(property, '5vmin', StyleUnit.ViewportMin, 5)
  testStyleUnitValue(property, '5vmax', StyleUnit.ViewportMax, 5)
  testStyleUnitValue(property, '5rem', StyleUnit.RootEm, 5)
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
