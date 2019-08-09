/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { Stage } from '../src/Stage'

let stage

describe('Stage', () => {
  beforeEach(() => {
    stage = new Stage()
    stage.init({ adapter: 'light-source-ref' })
  })
  afterEach(() => {
    stage[Symbol.for('destroy')]()
    stage = null
  })
  describe('fps', () => {
    it('should...', () => {
      assert.isTrue(true)
    })
  })
})
