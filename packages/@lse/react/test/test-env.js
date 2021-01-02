/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage, logger, LogLevel } from '@lse/core'
import { shutdown } from '@lse/react/reconciler'
import React from 'react'
import chai from 'chai'
import { getActiveContainers, render } from '../src/renderer.js'

const { assert } = chai

before(() => {
  logger.setLogLevel(LogLevel.OFF)

  stage.on('destroying', (event) => {
    shutdown()
  })

  stage.loadPlugin('platform:ref')
  stage.loadPlugin('audio:ref')
  stage.createScene()
  stage.start()
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

export const container = () => stage.getScene().root
