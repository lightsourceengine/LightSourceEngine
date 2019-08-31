/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { Style } from 'light-source'
import React from 'react'
import { renderAsync, root, beforeSceneTest, afterSceneTest } from './test-env'

describe('BoxElement', () => {
  beforeEach(beforeSceneTest)
  afterEach(afterSceneTest)
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
  describe('prop: waypoint', () => {
    it('should set waypoint to horizontal navigation', async () => {
      await renderAsync(<box waypoint='horizontal' />)

      assert.equal(root().children[0].waypoint.tag, 'horizontal')
    })
    it('should set waypoint to vertical navigation', async () => {
      await renderAsync(<box waypoint='vertical' />)

      assert.equal(root().children[0].waypoint.tag, 'vertical')
    })
    it('', async () => {
      const customWaypoint = { tag: 'custom', resolve () {}, navigate () {} }

      await renderAsync(<box waypoint={customWaypoint} />)

      assert.strictEqual(root().children[0].waypoint, customWaypoint)
    })
    it('should throw error for an invalid waypoint tag', async () => {
      try {
        await renderAsync(<box waypoint='test' />)
      } catch (e) {
        return
      }

      assert.fail('expected exception')
    })
  })
})