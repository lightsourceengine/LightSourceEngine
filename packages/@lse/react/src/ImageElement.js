/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element.js'

const kImageProps = [
  'onLoad',
  'onError',
  'src'
]

/**
 * Maps &lt;img&gt; element to an ImageSceneNode.
 *
 * @memberof module:@lse/react
 * @extends module:@lse/react.Element
 * @hideconstructor
 */
class ImageElement extends Element {
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

export { ImageElement }
