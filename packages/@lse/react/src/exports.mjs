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

import { stage } from '@lse/core'
import { shutdown } from 'react-reconciler-standalone'
import { render, findSceneNode, findElement } from './renderer.mjs'
import { letThereBeLight } from './letThereBeLight.mjs'
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

stage.once('destroying', (event) => {
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
