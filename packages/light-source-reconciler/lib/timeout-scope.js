/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
