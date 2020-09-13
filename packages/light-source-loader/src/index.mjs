/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */
import babel from '@babel/core'

const { transformAsync } = babel

export const isJsx = (url) => url.endsWith('.jsx') || url.endsWith('.mjsx')

export const getFormatJsx = async (url, context, defaultGetFormat) =>
  isJsx(url) ? { format: 'module' } : defaultGetFormat(url, context, defaultGetFormat)

export const babelTransformSource = async (source, context, defaultTransformSource) => {
  if (context.format === 'commonjs' || context.format === 'module') {
    const { code } = await transformAsync(source)

    source = code
  }

  return defaultTransformSource(source, context, defaultTransformSource)
}
