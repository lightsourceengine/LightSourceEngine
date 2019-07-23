/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Style } from 'light-source'
import { emptyObject } from './emptyObject'

export class Element {
  constructor (node, props) {
    // TODO: assert node
    this.node = node
    this.updateProps(emptyObject, props)
  }

  updateProps (oldProps, newProps) {
    if (oldProps.style !== newProps.style) {
      const { style } = newProps

      this.node.style = style
        ? style instanceof Style ? style : new Style(style)
        : style
    }

    this.props = newProps
  }

  appendChild (child, before) {
    if (before) {
      this.node.insertBefore(child.node, before.node)
    } else {
      this.node.appendChild(child.node)
    }
  }

  removeChild (child) {
    this.node.removeChild(child.node)
  }
}
