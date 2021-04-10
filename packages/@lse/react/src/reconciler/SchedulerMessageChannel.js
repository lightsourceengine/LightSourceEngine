/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
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
