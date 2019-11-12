/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { Style } from '../../src/addon'
import { getRotateAngle, isRotate, rotate } from '../../src/style/transform'

const style = (obj) => Object.assign(new Style(), obj)

const testStyleValue = (name, value, expectedValue) => {
  assert.deepEqual(style({ [name]: value })[name], expectedValue)
}

const testStyleValueEmpty = (name, value) => {
  assert.sameOrderedMembers(style({ [name]: value })[name], [undefined, Style.UnitUndefined])
}

const testStyleValueNull = (name, value) => {
  assert.isNull(style({ [name]: value })[name])
}

const testStyleUnitValue = (name, value, expectedUnit, expectedValue) => {
  assert.sameOrderedMembers(style({ [name]: value })[name], [expectedValue, expectedUnit])
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
        testStyleValue(property, 'blanchedalmond', 0xFFFFEBCD)
        testStyleValue(property, 'BLANCHEDALMOND', 0xFFFFEBCD)
        testStyleValue(property, 0xFFEBCD, 0xFFEBCD)
        testStyleValue(property, 0xFFFFEBCD, 0xFFFFEBCD)
        testStyleValue(property, '#FFEBCD', 0xFFFFEBCD)
        testStyleValue(property, '#ffebcd', 0xFFFFEBCD)
        testStyleValue(property, '#FFF', 0xFFFFFFFF)
        testStyleValue(property, '#fff', 0xFFFFFFFF)
        testStyleValue(property, '#ffffebcd', 0xFFFFEBCD)
        testStyleValue(property, '#ffff', 0xFFFFFFFF)
      }
    })
    it('should return 0 for invalid color values', () => {
      for (const property of colorProperties) {
        testStyleValue(property, '', undefined)
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
      testInvalidEnumProperty(property, enums[0])
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
      testInvalidPositionProperty(property)
    })
  })
  describe('backgroundPositionY property', () => {
    const property = 'backgroundPositionY'
    it('should set value', () => {
      testPositionProperty(property, yDirection)
    })
    it('should reject invalid value', () => {
      testInvalidPositionProperty(property)
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
      for (const input of [-1, '10', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
      }
    })
  })
  describe('fontStyle property', () => {
    const property = 'fontStyle'
    const enums = ['normal', 'italic']
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
      testStyleUnitValue(property, 100, Style.UnitPoint, 100)
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
      for (const input of [-1, '10', null, {}, [], undefined, NaN]) {
        testStyleValueEmpty(property, input)
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
      testInvalidPositionProperty(property)
    })
  })
  describe('objectPositionY property', () => {
    const property = 'objectPositionY'
    it('should set value', () => {
      testPositionProperty(property, yDirection)
    })
    it('should reject invalid value', () => {
      testInvalidPositionProperty(property)
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
      assert.lengthOf(style({}).transform, 0)
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
      assert.sameOrderedMembers(getRotateAngle(transformValue), [90, Style.UnitDegree])
    })
    it('should assignable to transform value', () => {
      const s = style({ transform: rotate('90deg') })

      assert.lengthOf(s.transform, 1)

      const transformValue = s.transform[0]

      assert.isTrue(isRotate(transformValue))
      assert.sameOrderedMembers(getRotateAngle(transformValue), [90, Style.UnitDegree])
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
      testInvalidPositionProperty(property)
    })
  })
  describe('transformOriginY property', () => {
    const property = 'transformOriginY'
    it('should set value', () => {
      testPositionProperty(property, yDirection)
    })
    it('should reject invalid value', () => {
      testInvalidPositionProperty(property)
    })
  })
  describe('backgroundImage property', () => {
    const property = 'backgroundImage'
    const testUri = 'test.jpg'
    const testId = 'test-id'
    it('should be assignable to a string uri', () => {
      testStyleValue(property, testUri, { id: testUri, uri: testUri })
    })
    it('should set from uri property', () => {
      testStyleValue(property, { uri: testUri }, { id: testUri, uri: testUri })
    })
    it('should set from uri property and override id', () => {
      testStyleValue(property, { id: testId, uri: testUri }, { id: testId, uri: testUri })
    })
    it('should set from uri property and reject invalid id', () => {
      testStyleValue(property, { id: 3, uri: testUri }, { id: testUri, uri: testUri })
    })
    it('should set from uri property and use width & height', () => {
      testStyleValue(property, { uri: testUri, width: 10, height: 20 },
        { id: testUri, uri: testUri, width: 10, height: 20 })
    })
    it('should  set from uri property and use int capInsets', () => {
      testStyleValue(property, { uri: testUri, capInsets: 10 },
        { id: testUri, uri: testUri, capInsets: { top: 10, right: 10, bottom: 10, left: 10 } })
    })
    it('should  set from uri property and use full capInsets', () => {
      const capInsets = { top: 1, right: 2, bottom: 3, left: 4 }
      testStyleValue(property, { uri: testUri, capInsets }, { id: testUri, uri: testUri, capInsets })
    })
    it('should reject invalid value', () => {
      const inputs = [
        null,
        undefined,
        3,
        '',
        NaN,
        {},
        { id: 'x' },
        { uri: 3 }
      ]

      inputs.forEach(input => testStyleValueNull(property, input))
    })
  })
  describe('zIndex property', () => {
    const property = 'zIndex'
    it('should set positive value', () => {
      testStyleUnitValue(property, 1, Style.UnitPoint, 1)
    })
    it('should set negative value', () => {
      testStyleUnitValue(property, -1, Style.UnitPoint, -1)
    })
    it('should reject invalid value', () => {
      const inputs = [
        '',
        '3',
        null,
        undefined,
        NaN,
        {}
      ]

      inputs.forEach(i => testStyleValueEmpty(property, i))
    })
  })
})

const xDirection = 1
const yDirection = 2

const testPositionProperty = (property, direction) => {
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

  if (direction === xDirection) {
    testStyleUnitValue(property, 'left', Style.UnitAnchor, 'left')
    testStyleUnitValue(property, 'right', Style.UnitAnchor, 'right')
  } else if (direction === yDirection) {
    testStyleUnitValue(property, 'top', Style.UnitAnchor, 'top')
    testStyleUnitValue(property, 'bottom', Style.UnitAnchor, 'bottom')
  } else {
    assert.fail()
  }
}

const testInvalidPositionProperty = (property) => {
  for (const input of ['', null, {}, [], undefined, NaN]) {
    testStyleValueEmpty(property, input)
  }
}

const testDimensionProperty = (property) => {
  testStyleUnitValue(property, 50, Style.UnitPoint, 50)
  testStyleUnitValue(property, '100%', Style.UnitPercent, 100)
  testStyleUnitValue(property, '50px', Style.UnitPoint, 50)
  testStyleUnitValue(property, '0px', Style.UnitPoint, 0)
  testStyleUnitValue(property, '5vw', Style.UnitViewportWidth, 5)
  testStyleUnitValue(property, '5vh', Style.UnitViewportHeight, 5)
  testStyleUnitValue(property, '5vmin', Style.UnitViewportMin, 5)
  testStyleUnitValue(property, '5vmax', Style.UnitViewportMax, 5)
  testStyleUnitValue(property, '5rem', Style.UnitRootEm, 5)
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
