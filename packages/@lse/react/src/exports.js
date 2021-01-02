/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage } from '@lse/core'
import { shutdown } from '@lse/react/reconciler'
import { render, findSceneNode, findElement } from './renderer.js'
import { letThereBeLight } from './letThereBeLight.js'
import { version as reactVersion } from 'react'

stage.once('destroyed', (event) => {
  shutdown()
})

export {
  render,
  findSceneNode,
  findElement,
  letThereBeLight,
  reactVersion
}

export const version = '$LSE_REACT_VERSION'

// export to support jsx transpilation using jsx-runtime
export { createElement } from 'react'
