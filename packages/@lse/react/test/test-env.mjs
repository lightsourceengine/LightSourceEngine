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

import { stage, logger, Constants } from '@lse/core'
import { shutdown } from '@lse/react/reconciler'
import React from 'react'
import chai from 'chai'
import { getActiveContainers, render } from '../src/renderer.mjs'

const { assert } = chai

before(() => {
  logger.setLogLevel(Constants.LogLevel.OFF)

  stage.on('destroying', (event) => {
    shutdown()
  })

  stage.configure({
    plugin: [
      Constants.PluginId.REF,
      Constants.PluginId.REF_AUDIO
    ],
    scene: {
      title: 'Test Scene'
    }
  })
})

after(async () => {
  stage.stop()
  stage.$destroy()
})

class Catch extends React.Component {
  componentDidCatch (error, info) {
    this.props.context.caught = error
  }

  render () {
    return this.props.children
  }
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

export const renderAsync = async (component) => {
  const context = { caught: null }

  await new Promise(resolve => {
    render(container(), <Catch context={context}>{component}</Catch>, () => resolve())
  })

  if (context.caught !== null) {
    throw context.caught
  }
}

export const beforeEachTestCase = () => {
}

export const afterEachTestCase = async () => {
  const promises = []

  for (const container of getActiveContainers()) {
    promises.push(new Promise((resolve) => render(container, null, () => resolve())))
  }

  await Promise.allSettled(promises)

  if (stage.scene) {
    // TODO: active node should be cleaned up automatically
    stage.scene.$setActiveNode(null)
    assert.lengthOf(stage.scene.root.children, 0)
  }
}

export const container = () => stage.$scene.root
