/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Stage } from './stage/Stage'
import { Style } from './style/Style'

const errorHandler = obj => {
  if (obj) {
    obj.message && console.log(obj.message)
    obj.stack && console.log(obj.stack)
  }

  process.exit()
};

['SIGINT', 'SIGUSR1', 'SIGUSR2', 'uncaughtException', 'unhandledRejection'].forEach(
  e => process.on(e, errorHandler))

export { Style }
export const stage = new Stage()
