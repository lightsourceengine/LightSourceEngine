/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Stage } from './stage/Stage'
import { createStyleSheet } from './style/createStyleSheet'
import { style } from './style/style'
import { rgba } from './style/rgba'
import { rgb } from './style/rgb'
import { ScanCode } from './input/ScanCode'
import { waypoint } from './scene/waypoint'
import { Mapping } from './input/Mapping'
import { Direction } from './input/Direction'
import { Key } from './input/Key'
import { Style } from './addon'
import { absoluteFill } from './style/absoluteFill'
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
} from './style/transform'

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
  // style
  Style,
  style,
  rgb,
  rgba,
  createStyleSheet,
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
  absoluteFill,

  // stage/scene
  stage,
  waypoint,
  Direction,

  // input
  Mapping,
  Key,
  ScanCode
}
