/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Stage } from './stage/Stage.js'

const errorHandler = obj => {
  if (obj) {
    obj.message && console.log(obj.message)
    obj.stack && console.log(obj.stack)
  }

  process.exit()
};

['SIGINT', 'uncaughtException', 'unhandledRejection'].forEach(e => process.on(e, errorHandler))

/// ////////////////////////////////////////////////////////////////////////////
/// Stage Exports
/// ////////////////////////////////////////////////////////////////////////////

export const stage = new Stage()
export { PluginType } from './stage/PluginType.js'
export { Direction } from './input/Direction.js'
export { waypoint } from './scene/waypoint.js'

/// ////////////////////////////////////////////////////////////////////////////
/// Style Exports
/// ////////////////////////////////////////////////////////////////////////////

export { createStyle } from './style/createStyle.js'
export { createStyleSheet } from './style/createStyleSheet.js'
export { MixinRegistry } from './style/MixinRegistry.js'
export { ShorthandRegistry } from './style/ShorthandRegistry.js'
export { rgba } from './style/rgba.js'
export { rgb } from './style/rgb.js'
export { StyleAnchor, StyleUnit, StyleTransform } from './addon/index.js'
export {
  translate,
  scale,
  rotate,
  scaleX,
  scaleY,
  translateX,
  translateY,
  isRotate,
  isScale,
  isTranslate,
  getRotateAngle,
  getTranslateX,
  getTranslateY,
  getScaleX,
  getScaleY
} from './style/transform.js'

/// ////////////////////////////////////////////////////////////////////////////
/// Audio Exports
/// ////////////////////////////////////////////////////////////////////////////

export { AudioDecoderType } from './audio/AudioDecoderType.js'
export { AudioSourceType } from './audio/AudioSourceType.js'

/// ////////////////////////////////////////////////////////////////////////////
/// Misc Exports
/// ////////////////////////////////////////////////////////////////////////////

export { ScanCode } from './input/ScanCode.js'
export { Mapping } from './input/Mapping.js'
export { Key } from './input/Key.js'
export { AnalogKey } from './input/AnalogKey.js'

/// ////////////////////////////////////////////////////////////////////////////
/// Misc Exports
/// ////////////////////////////////////////////////////////////////////////////

export const version = '$LSE_VERSION'
export { LogLevel, logger } from './addon/index.js'
