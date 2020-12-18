/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
      assert.isUndefined(node.style.fontSize[0])
    })
  })
})
