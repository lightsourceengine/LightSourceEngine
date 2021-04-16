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

import { getSceneNodeInstanceCount, logger, LogLevel } from '../src/addon/index.mjs'
import { PluginId } from '../src/addon/PluginId.mjs'
import { Stage } from '../src/stage/Stage.mjs'
import chai from 'chai'

const { assert } = chai
const nodes = []
let stage = null

before(() => {
  logger.setLogLevel(LogLevel.OFF)

  stage = new Stage()

  stage.configure({
    plugin: [
      PluginId.REF,
      PluginId.REF_AUDIO
    ],
    scene: {
      title: 'Test Scene',
      width: 1280,
      height: 720
    }
  })

  test.stage = stage
  test.scene = stage.$scene
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
  stage.$destroy()
  stage = null
})

export const test = { stage: null, scene: null, root: null }

export const beforeSceneTest = () => {
  return test.scene
}

export const afterSceneTest = () => {
  // TODO: active node should be cleaned up automatically
  test.scene.$setActiveNode(null)

  nodes.forEach(n => n.destroy())
  nodes.length = 0

  assert.lengthOf(test.scene.root.children, 0)
  assert.equal(getSceneNodeInstanceCount(), 1,
    'test has leaked SceneNode instances')
}

export const rejects = async (subject) => {
  let f = () => {}

  try {
    await (typeof subject === 'function' ? subject() : subject)
  } catch (e) {
    f = () => { throw e }
  } finally {
    assert.throws(f)
  }
}
