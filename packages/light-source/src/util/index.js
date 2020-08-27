/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { logger } from '../addon'
import { EventEmitter } from './EventEmitter'
import { performance } from 'perf_hooks'

export const { now } = performance

export const logexcept = (func, site) => {
  try {
    func()
  } catch (e) {
    logger.error(e.message, site)
  }
}

export const fileuri = (path, queryParams = {}) => {
  if (!path || typeof path !== 'string') {
    return ''
  }

  let queryString = ''

  for (const param of Object.keys(queryParams)) {
    queryString = `${queryString}${queryString ? '&' : ''}${param}=${encodeURIComponent(queryParams[param].toString())}`
  }

  if (path.search(/[+%]/) >= 0) {
    path = encodeURIComponent(path)
  }

  return `file:${path}${queryString && '?'}${queryString}`
}

// Math.clamp()
export const clamp = (val, min, max) => val > max ? max : val < min ? min : val

export const isNumber = (value) => ((typeof value === 'number') && (value - value === 0))

export const symbolFor = Symbol.for

export const symbolKeyFor = Symbol.keyFor

export const emptyArray = Object.freeze([])

export { EventEmitter }
