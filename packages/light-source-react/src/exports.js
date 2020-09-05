/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage } from 'light-source'
import { shutdown } from 'light-source-reconciler'
import { render, findSceneNode, findElement } from './renderer'
import { letThereBeLight } from './letThereBeLight'

stage.once('destroyed', (event) => {
  shutdown()
})

export { render, findSceneNode, findElement, letThereBeLight }
