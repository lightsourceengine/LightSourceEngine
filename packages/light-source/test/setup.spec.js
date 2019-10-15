/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Logger } from '../src/addon'

before(() => {
  Logger.setLogLevel(Logger.LogLevelOff)
})
