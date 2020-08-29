/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { createStyleSheet } from '../../src/style/createStyleSheet'
import { Style, StyleUnit } from '../../src/addon'
import { rgb } from '../../src/exports'

describe('createStyleSheet()', () => {
  it('should return an empty style sheet when spec is an empty object', () => {
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
  it('should throw Error when no argument passed', () => {
    assert.throws(() => createStyleSheet())
  })
  it('should throw Error when invalid argument passed', () => {
    for (const invalidSpec of ['', 1, null, undefined, NaN]) {
      assert.throws(() => createStyleSheet(invalidSpec))
    }
  })
  it('should create style sheet with local mixins', () => {
    const sheet = createStyleSheet({
      '%mixin': {
        flexDirection: 'row',
        width: 10
      },
      test: {
        height: 20,
        '@extend': '%mixin'
      }
    })

    assert.lengthOf(Object.keys(sheet), 1)
    assert.deepInclude(sheet.test, {
      width: [10, StyleUnit.Point],
      height: [20, StyleUnit.Point],
      flexDirection: 'row'
    })
  })
  it('should create style sheet with extend of local named style', () => {
    const sheet = createStyleSheet({
      mixin: {
        flexDirection: 'row',
        width: 10
      },
      test: {
        height: 20,
        '@extend': 'mixin'
      }
    })

    assert.lengthOf(Object.keys(sheet), 2)
    assert.deepInclude(sheet.test, {
      width: [10, StyleUnit.Point],
      height: [20, StyleUnit.Point],
      flexDirection: 'row'
    })
  })
  it('should create style sheet with global mixin', () => {
    const sheet = createStyleSheet({
      test: {
        height: 20,
        '@extend': '%absoluteFill'
      }
    })

    assert.lengthOf(Object.keys(sheet), 1)
    assert.deepInclude(sheet.test, {
      top: [0, StyleUnit.Point],
      right: [0, StyleUnit.Point],
      bottom: [0, StyleUnit.Point],
      left: [0, StyleUnit.Point],
      position: 'absolute'
    })
  })
  it('should create style sheet with extend list', () => {
    const sheet = createStyleSheet({
      '%mixin': {
        margin: 10
      },
      test: {
        '@extend': ['%absoluteFill', '%mixin']
      }
    })

    assert.lengthOf(Object.keys(sheet), 1)
    assert.deepInclude(sheet.test, {
      top: [0, StyleUnit.Point],
      right: [0, StyleUnit.Point],
      bottom: [0, StyleUnit.Point],
      left: [0, StyleUnit.Point],
      position: 'absolute',
      margin: [10, StyleUnit.Point]
    })
  })
  it('should create style sheet with extend of local mixin', () => {
    const sheet = createStyleSheet({
      '%mixin': {
        margin: 10
      },
      test: {
        margin: 20,
        '@extend': '%mixin'
      }
    })

    assert.lengthOf(Object.keys(sheet), 1)
    assert.deepInclude(sheet.test, {
      margin: [20, StyleUnit.Point]
    })
  })
  it('should create style sheet with extend of multiple local mixins', () => {
    const sheet = createStyleSheet({
      '%mixin1': {
        paddingTop: 10
      },
      '%mixin2': {
        paddingBottom: 20
      },
      test: {
        '@extend': ['%mixin1', '%mixin2']
      }
    })

    assert.lengthOf(Object.keys(sheet), 1)
    assert.deepInclude(sheet.test, {
      paddingTop: [10, StyleUnit.Point],
      paddingBottom: [20, StyleUnit.Point]
    })
  })
  it('should create style sheet with nested extend', () => {
    const sheet = createStyleSheet({
      '%mixin1': {
        paddingTop: 10
      },
      '%mixin2': {
        paddingBottom: 20,
        '@extend': '%mixin1'
      },
      test: {
        '@extend': '%mixin2'
      }
    })

    assert.lengthOf(Object.keys(sheet), 1)
    assert.deepInclude(sheet.test, {
      paddingTop: [10, StyleUnit.Point],
      paddingBottom: [20, StyleUnit.Point]
    })
  })
  it('should throw Error when a cycle is found in @extend chain', () => {
    assert.throws(() => createStyleSheet({
      '%mixin1': {
        paddingTop: 10,
        '@extend': 'test'
      },
      test: {
        '@extend': '%mixin1'
      }
    }))
  })
  it('should throw Error when style name starts with @', () => {
    assert.throws(() => createStyleSheet({ '@test': { border: 3 } }))
  })
  it('should throw Error when style value is null', () => {
    assert.throws(() => createStyleSheet({ test: null }))
  })
  it('should throw Error when style value is undefined', () => {
    assert.throws(() => createStyleSheet({ test: undefined }))
  })
})
