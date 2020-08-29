/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage, logger, LogLevel } from 'light-source'
import { shutdown } from 'light-source-reconciler'
import React from 'react'
import { assert } from 'chai'
import { getActiveContainers, render } from '../src/renderer'

before(() => {
  logger.setLogLevel(LogLevel.Off)
  stage.loadPlugin('light-source-ref')
  stage.loadPlugin('light-source-ref-audio')
  stage.createScene()
  stage.start()
})

after(async () => {
  stage.stop()
  shutdown()
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
}

export const container = () => stage.getScene().root
