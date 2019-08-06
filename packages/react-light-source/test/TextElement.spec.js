/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import React from 'react'
import { renderAsync, root, before, after } from './test-env'

describe('TextElement', () => {
  beforeEach(before)
  afterEach(after)
  describe('prop: text', () => {
    it('should set text', async () => {
      await renderAsync(<text>Hello</text>)

      assert.equal(root().children[0].text, 'Hello')
    })
    it('should set empty text', async () => {
      await renderAsync(<text />)

      assert.equal(root().children[0].text, '')
    })
  })
})
