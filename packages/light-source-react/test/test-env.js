/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage } from 'light-source'
import { ReactRenderer } from '../src/ReactRenderer'
import { shutdown } from 'light-source-reconciler'
import { Reconciler } from '../src/Reconciler'
import React from 'react'

before(() => {
  stage.loadPlugin('light-source-ref')
  stage.loadPlugin('light-source-ref-audio')
  state.scene = stage.createScene()
  stage.start()
})

// After all tests have run, remove all node event loop references held by the React Reconciler so mocha can close.
after(() => {
  shutdown()
  stage.quit()
})

const state = {
  scene: null,
  container: null
}

class Catch extends React.Component {
  componentDidCatch (error, info) {
    this.props.context.caught = error
  }

  render () {
    return this.props.children
  }
}

export const renderAsync = async (component) => {
  const context = { caught: null }

  await new Promise(resolve => {
    container().render(<Catch context={context}>{component}</Catch>, () => resolve())
  })

  if (context.caught !== null) {
    throw context.caught.message
  }
}

export const beforeSceneTest = () => {
  state.container = new ReactRenderer(Reconciler(state.scene), state.scene.root)
}

export const afterSceneTest = async () => {
  await new Promise((resolve) => container().disconnect(() => resolve()) || resolve())
  state.container = null
}

export const container = () => {
  return state.container
}

export const scene = () => state.scene
export const root = () => state.scene.root
