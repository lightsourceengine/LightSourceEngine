/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Stage } from './stage/Stage.js'

/**
 * @module @lse/core
 */

/**
 * @namespace module:@lse/core.$
 */

/**
 * @namespace module:@lse/core.core-enum
 */

for (const eventName of ['uncaughtException', 'unhandledRejection']) {
  process.on(eventName, ({ message, stack }) => {
    message && console.log(message)
    stack && console.log(stack)

    process.exit(1)
  })
}

process.on('SIGINT', () => process.exit(0))

/// ////////////////////////////////////////////////////////////////////////////
/// Top-Level Function and Object exports
/// ////////////////////////////////////////////////////////////////////////////

/**
 * Stage instance.
 *
 * @type {module:@lse/core.Stage}
 * @name module:@lse/core.stage
 */
export const stage = new Stage()

/**
 * Version
 *
 * @type {string}
 * @const
 * @name module:@lse/core.version
 */
export const version = '$LSE_VERSION'

export { logger } from './addon/index.js'

/// ////////////////////////////////////////////////////////////////////////////
/// Namespace exports
/// ////////////////////////////////////////////////////////////////////////////

export * as Input from './namespace-input.js'
export * as Style from './namespace-style.js'
export * as Constants from './namespace-constants.js'
