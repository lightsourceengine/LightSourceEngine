/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element.js'

/**
 * Maps &lt;text&gt; element to an TextSceneNode.
 *
 * @memberof module:@lse/react
 * @extends module:@lse/react.Element
 * @hideconstructor
 */
class TextElement extends Element {
  /**
   * @override
   */
  updateProps (oldProps, newProps) {
    super.updateProps(oldProps, newProps)

    if (oldProps.children !== newProps.children) {
      this.node.text = newProps.children
    }
  }
}

export { TextElement }
