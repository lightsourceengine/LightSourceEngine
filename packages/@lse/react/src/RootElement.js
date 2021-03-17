/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element.js'
import { emptyObject } from './emptyObject.js'

/**
 * Special wrapper element for wrapping the root SceneNode of the renderer's container.
 *
 * <p>Note, no element tag maps to this element. It is used for the reconciler to modify the roots children. The
 * reconciler will never set or update props on this element.</p>
 *
 * @memberof module:@lse/react
 * @extends module:@lse/react.Element
 * @hideconstructor
 */
class RootElement extends Element {
  constructor (node) {
    super(node, emptyObject)
  }
}

export { RootElement }
