/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { getSceneNodeInstanceCount, logger } from '../src/addon'
import { Stage } from '../src/stage/Stage'
import { assert } from 'chai'

const nodes = []
let stage = null

before(() => {
  logger.setLogLevel(logger.LogLevelOff)

  stage = new Stage()
  stage.loadPlugin('light-source-ref')
  stage.loadPlugin('light-source-ref-audio')
  stage.createScene({ width: 1280, height: 720 })
  stage.start()

  test.stage = stage
  test.scene = stage.getScene()
  test.root = test.scene.root

  // override createNode so scene tests can clean up nodes after each test
  test.scene.createNode = (tag) => {
    const n = Object.getPrototypeOf(test.scene).createNode.bind(test.scene)(tag)

    nodes.push(n)

    return n
  }
})

after(async () => {
  test.stage = test.scene = test.root = null
  stage.stop()
  stage = null
})

export const test = { stage: null, scene: null, root: null }

export const beforeSceneTest = () => {
  return test.scene
}

export const afterSceneTest = () => {
  test.scene.$setActiveNode(null)
  for (const n of test.root.children) {
    test.root.removeChild(n)
  }

  nodes.forEach(n => n.destroy())
  nodes.length = 0

  assert.equal(getSceneNodeInstanceCount(), 1, 'test has leaked SceneNode instances')
}
