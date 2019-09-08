/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Stage } from './stage/Stage'
import { Style } from './style/Style'
import { ScanCode } from './input/ScanCode'
import { waypoint } from './scene/waypoint'
import { Mapping } from './input/Mapping'
import { Key } from './input/Key'

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

  // stage/scene
  stage,
  waypoint,

  // input
  Mapping,
  Key,
  ScanCode
}
