/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element'

export class TextElement extends Element {
  updateProps (oldProps, newProps) {
    super.updateProps(oldProps, newProps)

    if (oldProps.children !== newProps.children) {
      this.node.text = newProps.children
    }
  }
}
