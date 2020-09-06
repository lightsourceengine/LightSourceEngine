/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element.js'

const kLinkProps = [
  'href',
  'rel',
  'as',
  'onLoad',
  'onError'
]

export class LinkElement extends Element {
  /**
   * @class LinkElement
   * @extends Element
   *
   * @classdesc
   *
   * Maps &lt;img&gt; element to an LinkSceneNode.
   */

  /**
   * @override
   */
  updateProps (oldProps, newProps) {
    const { node } = this

    this.props = newProps

    for (const name of kLinkProps) {
      if (oldProps[name] !== newProps[name]) {
        node[name] = newProps[name]
      }
    }

    node.fetch()
  }
}
