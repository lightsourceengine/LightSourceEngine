/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { afterSceneTest, beforeSceneTest } from '../test-env'
import { assert } from 'chai'
import { RootSceneNode } from '../../src/addon'
import { Style } from '../../src/addon'

describe('RootSceneNode', () => {
  let node
  let scene
  before(() => scene = beforeSceneTest())
  after(() => {
    node && node.destroy()
    scene = afterSceneTest()
  })
  describe('constructor()', () => {
    it('should create uninitialized node when passed an invalid Scene', () => {
      for (const input of [null, undefined, {}]) {
        assert.throws(() => new RootSceneNode())
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
