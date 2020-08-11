/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element'

const kImageProps = [
  'src',
  'onLoad',
  'onError'
]

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

    const { node } = this

    for (const name of kImageProps) {
      if (oldProps[name] !== newProps[name]) {
        node[name] = newProps[name]
      }
    }
  }
}
