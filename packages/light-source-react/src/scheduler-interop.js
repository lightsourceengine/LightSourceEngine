/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

// XXX: Expose global window for scheduler.
if (!global.window) {
  const { performance } = require('perf_hooks')

  global.window = {
    Date,
    setTimeout,
    clearTimeout,
    performance,
    requestAnimationFrame: () => {},
    cancelAnimationFrame: () => {}
  }
}

// XXX: Expose global MessageChannel for scheduler.
if (!global.MessageChannel) {
  const { MessageChannel } = require('worker_threads')
  let instance = null

  class MessageChannelSingleton {
    constructor () {
      if (instance) {
        throw Error('MessageChannel should be instantiated once!')
      }

      instance = new MessageChannel()
      delete global.window
    }

    get port1 () {
      return instance.port1
    }

    get port2 () {
      return instance.port2
    }

    static close () {
      instance.port1.close()
      instance.port2.close()
      instance = null
    }
  }

  global.MessageChannel = MessageChannelSingleton
}
