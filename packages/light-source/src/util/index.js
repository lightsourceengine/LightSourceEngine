/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { join } from 'path'

const resourceHost = 'file://resource/'

export const logexcept = (func, prefix) => {
  try {
    func()
  } catch (e) {
    console.log(prefix ? prefix + e.message : e.message)
  }
}

export const resolveUri = (uri, resourcePath) => {
  return uri.startsWith(resourceHost) ? join(resourcePath, uri.substr(resourceHost.length)) : uri
}

// Math.clamp()
export const clamp = (val, min, max) => val > max ? max : val < min ? min : val
