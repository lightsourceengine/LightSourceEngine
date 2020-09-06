/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element.js'

export class TextElement extends Element {
  /**
   * @class TextElement
   * @extends Element
   *
   * @classdesc
   *
   * Maps &lt;text&gt; element to an TextSceneNode.
   */

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
