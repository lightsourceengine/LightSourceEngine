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

import { Stage } from './stage/Stage.mjs'

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

export { logger } from './addon/index.mjs'

/// ////////////////////////////////////////////////////////////////////////////
/// Namespace exports
/// ////////////////////////////////////////////////////////////////////////////

export * as Input from './namespace-input.mjs'
export * as Style from './namespace-style.mjs'
export * as Constants from './namespace-constants.mjs'
