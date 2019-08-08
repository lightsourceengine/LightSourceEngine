/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage } from 'light-source'
import { ReactRenderer } from '../src/ReactRenderer'
import { Reconciler } from '../src/Reconciler'

const state = {
  scene: null,
  container: null
}

export const renderAsync = (component) => {
  return new Promise(resolve => { container().render(component, () => resolve()) })
}

export const beforeSceneTest = () => {
  stage.init({ adapter: 'light-source-ref' })
  state.scene = stage.createScene()
}

export const afterSceneTest = async () => {
  await new Promise((resolve) => container().disconnect(() => resolve()) || resolve())
  stage[Symbol.for('destroy')]()
  state.scene = null
  state.container = null
}

export const container = () => {
  if (!state.container) {
    state.container = new ReactRenderer(new Reconciler(state.scene), state.scene.root)
  }

  return state.container
}

export const scene = () => state.scene
export const root = () => state.scene.root
