/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage } from '@lse/core'
import { shutdown, reactReconcilerVersion } from '@lse/reconciler'
import { render, findSceneNode, findElement } from './renderer.js'
import { letThereBeLight } from './letThereBeLight.js'

stage.once('destroyed', (event) => {
  shutdown()
})

const { lightSourceReactVersion } = global // set by rollup from package.json
const { reactVersion } = global // set by rollup from package.json

export {
  render,
  findSceneNode,
  findElement,
  letThereBeLight,
  reactVersion,
  reactReconcilerVersion,
  lightSourceReactVersion
}
