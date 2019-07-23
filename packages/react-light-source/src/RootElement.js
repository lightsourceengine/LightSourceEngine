/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element'
import { emptyObject } from './emptyObject'

export class RootElement extends Element {
  constructor (node) {
    super(node, emptyObject)
  }

  updateProps (oldProps, newProps) {
    super.updateProps(oldProps, newProps)
  }
}
