/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage, EventName } from '@lse/core'
import { shutdown } from '@lse/react/reconciler'
import { render, findSceneNode, findElement } from './renderer.js'
import { letThereBeLight } from './letThereBeLight.js'
import { version as reactVersion } from 'react'

/**
 * @module @lse/react
 */

/**
 * React Version
 *
 * @type {string}
 * @const
 * @name module:@lse/react.reactVersion
 */

stage.once(EventName.onDestroying, (event) => {
  shutdown()
})

/**
 * Version
 *
 * @type {string}
 * @const
 * @name module:@lse/react.version
 */
export const version = '$LSE_REACT_VERSION'

// export to support jsx transpilation using jsx-runtime
export { createElement } from 'react'

export {
  render,
  findSceneNode,
  findElement,
  letThereBeLight,
  reactVersion
}
