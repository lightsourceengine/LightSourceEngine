/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { afterSceneTest, beforeSceneTest } from '.'
import { assert } from 'chai'
import { RootSceneNode } from '../src/addon'

describe('RootSceneNode', () => {
  before(beforeSceneTest)
  after(afterSceneTest)
  describe('constructor()', () => {
    it('should throw Error when arg is not a Scene', () => {
      for (const input of [null, undefined, {}]) {
        assert.throws(() => new RootSceneNode(input))
      }
    })
  })
})
