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

import { Element } from './Element.mjs'
import { Input } from '@lse/core'

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
        this.node.waypoint = Input.waypoint(newProps.waypoint)
      } else {
        this.node.waypoint = newProps.waypoint
      }
    }
  }
}

export { BoxElement }
