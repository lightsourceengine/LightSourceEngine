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
