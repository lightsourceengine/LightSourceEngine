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
