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

import { logger } from '../addon/index.js'
import { EventEmitter } from './EventEmitter.js'
import { performance } from 'perf_hooks'

const kDataUriPrefix = 'data:'

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

export const isPlainObject = (obj) => (typeof obj === 'object' && obj?.constructor === Object)

export const symbolFor = Symbol.for

export const symbolKeyFor = Symbol.keyFor

export const emptyArray = Object.freeze([])

export const emptyObject = Object.freeze({})

export const invertMap = (map) => new Map(Array.from(map, entry => entry.reverse()))

export const atob = (str) => Buffer.from(str, 'utf-8').toString('base64')

export const isDataUri = (uri) => typeof uri === 'string' && uri.startsWith(kDataUriPrefix)

export { EventEmitter }
