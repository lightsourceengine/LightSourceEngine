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
import { afterSceneTest, beforeSceneTest } from '../test-env.js'
import { RootSceneNode } from '../../src/scene/SceneNode.js'

const { assert } = chai

describe('RootSceneNode', () => {
  let node
  let scene
  beforeEach(() => { scene = beforeSceneTest() })
  afterEach(() => {
    node && node.destroy()
    node = null
    scene = afterSceneTest()
  })
  describe('constructor()', () => {
    it('should create uninitialized node when passed an invalid Scene', () => {
      for (const input of [null, undefined, {}]) {
        assert.throws(() => new RootSceneNode(input))
      }
    })
  })
  describe('style property', () => {
    it('should be empty after constructor', () => {
      node = new RootSceneNode(scene)
      assert.isTrue(node.style.fontSize.isUndefined())
    })
  })
})
