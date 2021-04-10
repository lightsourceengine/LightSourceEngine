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
import { createStyleClass } from '../../src/style/createStyleClass.js'
import { StyleUnit } from '../../src/addon/index.js'
import { StyleValue } from '../../src/style/StyleValue.js'
import { StyleClass } from '../../src/style/StyleClass.js'

const { assert } = chai

describe('createStyleClass()', () => {
  it('should create a new style sheet', () => {
    const style = createStyleClass({
      border: 10,
      margin: 15
    })

    assert.instanceOf(style, StyleClass)
    assert.deepEqual(style.border, new StyleValue(10, StyleUnit.Point))
    assert.deepEqual(style.margin, new StyleValue(15, StyleUnit.Point))
  })
  it('should process shorthand properties', () => {
    const style = createStyleClass({
      '@size': 50
    })

    assert.instanceOf(style, StyleClass)
    assert.deepEqual(style.width, new StyleValue(50, StyleUnit.Point))
    assert.deepEqual(style.height, new StyleValue(50, StyleUnit.Point))
  })
  it('should return spec if instanceof Style', () => {
    // TODO: remove
    const style = new StyleClass()

    assert.strictEqual(createStyleClass(style), style)
  })
  it('should return null for invalid spec', () => {
    for (const input of [null, undefined, '', 3, NaN]) {
      assert.isNull(createStyleClass(input))
    }
  })
  it('should throw Error when setting created class', () => {
    const styleClass = createStyleClass({ backgroundColor: 'red' })

    assert.throws(() => { styleClass.backgroundColor = 'blue' })
  })
})
