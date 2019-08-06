/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { Style } from 'light-source'
import React from 'react'
import { renderAsync, root, before, after } from './test-env'

describe('BoxElement', () => {
  beforeEach(before)
  afterEach(after)
  describe('prop: style', () => {
    it('should set style with Style object', async () => {
      await renderAsync(<box style={new Style({ backgroundColor: 'red' })} />)

      assert.equal(root().children[0].style.backgroundColor, 0xFF0000)
    })
    it('should set style with Object', async () => {
      await renderAsync(<box style={{ backgroundColor: 'red' }} />)

      assert.equal(root().children[0].style.backgroundColor, 0xFF0000)
    })
  })
})
