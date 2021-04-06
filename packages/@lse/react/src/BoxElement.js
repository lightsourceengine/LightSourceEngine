/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element.js'
import { waypoint } from '@lse/core'

/**
 * Maps &lt;box&gt; element to a BoxSceneNode.
 *
 * @memberof module:@lse/react
 * @extends module:@lse/react.Element
 * @hideconstructor
 */
class BoxElement extends Element {
  /**
   * @override
   */
  updateProps (oldProps, newProps) {
    super.updateProps(oldProps, newProps)

    if (oldProps.waypoint !== newProps.waypoint) {
      if (typeof newProps.waypoint === 'string') {
        this.node.waypoint = waypoint(newProps.waypoint)
      } else {
        this.node.waypoint = newProps.waypoint
      }
    }
  }
}

export { BoxElement }
