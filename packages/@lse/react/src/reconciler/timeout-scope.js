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

const timeoutHandles = new Set()
const { setTimeout, clearTimeout } = global

const setTimeoutWrapped = (...args) => {
  const handle = setTimeout(...args)
  timeoutHandles.set(handle)
  return handle
}

const clearTimeoutWrapped = (handle) => {
  timeoutHandles.delete(handle)
  clearTimeout(handle)
}

const closePendingTimeouts = () => {
  for (const handle of timeoutHandles) {
    clearTimeout(handle)
  }
  timeoutHandles.clear()
}

export { setTimeoutWrapped as setTimeout, clearTimeoutWrapped as clearTimeout, closePendingTimeouts }
