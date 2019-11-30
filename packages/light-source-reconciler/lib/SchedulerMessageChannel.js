/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { MessageChannel } from 'worker_threads'

// Secret singleton wrapper of MessageChannel.
//
// scheduler creates one MessageChannel instance on import. No API is provided to close the channel and it's associated
// ports, which is problematic for program shutdown. SchedulerMessageChannel is presented to the scheduler as a
// normal MessageChannel class, but internally it holds a singleton instance. SchedulerMessageChannel exposes API
// to close the channel ports.
//
// Not ideal, but it works.

class SchedulerMessageChannel {
  constructor () {
    if (SchedulerMessageChannel.instance) {
      throw Error('SchedulerMessageChannel can only be instantiated once!')
    }

    SchedulerMessageChannel.instance = new MessageChannel()
  }

  get port1 () {
    return SchedulerMessageChannel.instance.port1
  }

  get port2 () {
    return SchedulerMessageChannel.instance.port2
  }
}

SchedulerMessageChannel.close = () => {
  const { instance } = SchedulerMessageChannel

  if (instance) {
    instance.port1.close()
    instance.port2.close()
    SchedulerMessageChannel.instance = null
  }
}

export default SchedulerMessageChannel
