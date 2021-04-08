/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { CStyle, resetStyle } from '../addon/index.js'

/**
 * A style object.
 *
 * @memberof module:@lse/core
 * @hideconstructor
 */
class StyleInstance extends CStyle {
  reset () {
    resetStyle(this)
  }
}

export { StyleInstance }
