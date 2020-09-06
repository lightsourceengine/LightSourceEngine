/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Stage } from './stage/Stage.js'
import { createStyleSheet } from './style/createStyleSheet.js'
import { ShorthandRegistry } from './style/ShorthandRegistry.js'
import { MixinRegistry } from './style/MixinRegistry.js'
import { createStyle } from './style/createStyle.js'
import { rgba } from './style/rgba.js'
import { rgb } from './style/rgb.js'
import { ScanCode } from './input/ScanCode.js'
import { waypoint } from './scene/waypoint.js'
import { Mapping } from './input/Mapping.js'
import { MappingType } from './input/MappingType.js'
import { PluginType } from './stage/PluginType.js'
import { Direction } from './input/Direction.js'
import { Key } from './input/Key.js'
import { StyleAnchor, StyleUnit, StyleTransform, LogLevel, logger } from './addon/index.js'
import { AudioDecoderType } from './audio/AudioDecoderType.js'
import { AudioSourceType } from './audio/AudioSourceType.js'
import { InputDeviceType } from './input/InputDeviceType.js'
import {
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

const errorHandler = obj => {
  if (obj) {
    obj.message && console.log(obj.message)
    obj.stack && console.log(obj.stack)
  }

  process.exit()
};

['SIGINT', 'uncaughtException', 'unhandledRejection'].forEach(e => process.on(e, errorHandler))

const stage = new Stage()

export {
  // Style
  StyleUnit,
  StyleTransform,
  StyleAnchor,
  ShorthandRegistry,
  createStyle,
  rgb,
  rgba,
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
  getScaleY,

  // StyleSheet
  createStyleSheet,
  MixinRegistry,

  // Stage
  stage,
  waypoint,
  Direction,
  PluginType,

  // Input
  MappingType,
  InputDeviceType,
  Key,
  ScanCode,
  Mapping,

  // Logger
  logger,
  LogLevel,

  // Audio
  AudioDecoderType,
  AudioSourceType
}
