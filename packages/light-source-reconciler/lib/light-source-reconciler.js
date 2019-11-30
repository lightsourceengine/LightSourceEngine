/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import ReactReconciler from 'react-reconciler'
import SchedulerMessageChannel from './SchedulerMessageChannel'
import { closePendingTimeouts } from './timeout-scope'

const shutdown = () => {
  SchedulerMessageChannel.close()
  closePendingTimeouts()
}

export { ReactReconciler, shutdown }
