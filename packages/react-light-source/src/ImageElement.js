/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element'

export class ImageElement extends Element {
  updateProps (oldProps, newProps) {
    super.updateProps(oldProps, newProps)

    // TODO: deeper compare of src once the structure is fleshed out
    if (oldProps.src !== newProps.src) {
      this.node.src = newProps.src
    }

    if (oldProps.onLoad !== newProps.onLoad) {
      this.node.onLoad = newProps.onLoad
    }
  }
}
