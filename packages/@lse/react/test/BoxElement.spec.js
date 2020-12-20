/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import React from 'react'
import { renderAsync, rejects, beforeEachTestCase, container, afterEachTestCase } from './test-env.js'

const { assert } = chai

let root

describe('BoxElement', () => {
  beforeEach(() => {
    beforeEachTestCase()
    root = container()
  })
  afterEach(async () => {
    root = null
    await afterEachTestCase()
  })
  describe('prop: waypoint', () => {
    it('should set waypoint to horizontal navigation', async () => {
      await renderAsync(<box waypoint='horizontal' />)

      assert.equal(root.children[0].waypoint.tag, 'horizontal')
    })
    it('should set waypoint to vertical navigation', async () => {
      await renderAsync(<box waypoint='vertical' />)

      assert.equal(root.children[0].waypoint.tag, 'vertical')
    })
    it('should set custom waypoint', async () => {
      const customWaypoint = { tag: 'custom', resolve () {}, navigate () {} }

      await renderAsync(<box waypoint={customWaypoint} />)

      assert.strictEqual(root.children[0].waypoint, customWaypoint)
    })
    it('should throw error for an invalid waypoint tag', async () => {
      await rejects(renderAsync(<box waypoint='test' />))
    })
  })
})