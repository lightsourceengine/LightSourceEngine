/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Stage } from '../src/stage/Stage'
import { getSceneNodeInstanceCount } from '../src/addon'
import { assert } from 'chai'

let stage
let scene
let nodes

export const beforeSceneTest = () => {
  nodes = []
  stage = new Stage()
  stage.init({ adapter: 'light-source-ref' })
  scene = stage.createScene({ width: 1280, height: 720 })

  return scene
}

export const afterSceneTest = () => {
  nodes.forEach(n => n.destroy())
  stage[Symbol.for('destroy')]()
  stage = null
  scene = null

  assert.equal(getSceneNodeInstanceCount(), 0, 'test has leaked SceneNode instances')
}

export const createNode = (tag) => {
  const n = scene.createNode(tag)

  nodes.push(n)

  return n
}
