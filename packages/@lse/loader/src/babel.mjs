/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { resolve } from './common.mjs'
import { transformAsync } from '@babel/core'
import { fileURLToPath } from 'url'

/**
 * @module @lse/loader/babel
 */

/**
 * Light Source Engine module loader with Babel transpilation.
 *
 * - Includes all features from the default Light Source Engine module loader.
 * - Adds support for file extensions: jsx, mjsx and cjsx.
 * - Adds Babel transpilation.
 *   - User must install at least @babel/core
 *   - User provides Babel configuration (.babelrc, babel.config.json, etc).
 *
 * @ignore
 */

const mjsExtensions = /\.mjsx$/
const cjsExtensions = /\.jsx$|\.cjsx$|_mocha$/
const formats = /commonjs|module/

export const getFormat = async (url, context, defaultGetFormat) => {
  if (cjsExtensions.test(url)) {
    return { format: 'commonjs' }
  } else if (mjsExtensions.test(url)) {
    return { format: 'module' }
  }

  return defaultGetFormat(url, context, defaultGetFormat)
}

export const transformSource = async (source, context, defaultTransformSource) => {
  let transformedSource

  if (formats.test(context.format)) {
    transformedSource = await transformAsync(source, { filename: fileURLToPath(context.url) })
  }

  return transformedSource ? { source: transformedSource.code } : defaultTransformSource(source, context, defaultTransformSource)
}

export { resolve }
