/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element'
import { emptyObject } from './emptyObject'

const getId = src => typeof src === 'string' ? src : src.id || src.uri || null

export class ImageElement extends Element {
  /**
   * @class ImageElement
   * @extends Element
   *
   * @classdesc
   *
   * Maps &lt;img&gt; element to an ImageSceneNode.
   */

  /**
   * @override
   */
  updateProps (oldProps, newProps) {
    super.updateProps(oldProps, newProps)

    if (getId(oldProps.src || emptyObject) !== getId(newProps.src || emptyObject)) {
      this.node.src = newProps.src
    }

    if (oldProps.onLoad !== newProps.onLoad) {
      this.node.onLoad = newProps.onLoad
    }

    if (oldProps.onError !== newProps.onError) {
      this.node.onError = newProps.onError
    }
  }
}
