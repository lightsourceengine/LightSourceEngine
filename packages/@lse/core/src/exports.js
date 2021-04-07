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
/// Stage / Scene Exports
/// ////////////////////////////////////////////////////////////////////////////

/**
 * Stage instance.
 *
 * @type {module:@lse/core.Stage}
 * @name module:@lse/core.stage
 */
export const stage = new Stage()
export { PluginType } from './addon/PluginType.js'
export { PluginId } from './addon/PluginId.js'
export { Direction } from './input/Direction.js'
export { waypoint } from './scene/waypoint.js'

/// ////////////////////////////////////////////////////////////////////////////
/// Style Exports
/// ////////////////////////////////////////////////////////////////////////////

export { createStyle } from './style/createStyle.js'
export { createStyleSheet } from './style/createStyleSheet.js'
export { MixinRegistry } from './style/MixinRegistry.js'
export { ShorthandRegistry } from './style/ShorthandRegistry.js'
export { StyleAnchor, StyleFilter, StyleUnit, StyleTransform } from './addon/index.js'
export * as $ from './style/style-functions.js'

/// ////////////////////////////////////////////////////////////////////////////
/// Audio Exports
/// ////////////////////////////////////////////////////////////////////////////

export { AudioDecoderType } from './audio/AudioDecoderType.js'
export { AudioType } from './audio/AudioType.js'

/// ////////////////////////////////////////////////////////////////////////////
/// Input Exports
/// ////////////////////////////////////////////////////////////////////////////

export { Mapping } from './input/Mapping.js'
export { ScanCode } from './input/ScanCode.js'
export { Key } from './input/Key.js'
export { AnalogKey } from './input/AnalogKey.js'
export { Hat } from './input/Hat.js'

/// ////////////////////////////////////////////////////////////////////////////
/// Misc Exports
/// ////////////////////////////////////////////////////////////////////////////

/**
 * Version
 *
 * @type {string}
 * @const
 * @name module:@lse/core.version
 */
export const version = '$LSE_VERSION'
export { LogLevel, logger } from './addon/index.js'
