/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import babel from '@babel/core'
import { getFormatJsx, isJsx } from './index.mjs'

/**
 * Module loader that limits babel transpilation to jsx and mjsx files only.
 *
 * The babel configuration is embedded, and not configurable. This is expected to be a common case. Light Source
 * Engine requires node 14+, which supports much of the ES2020 standard out of the box. For most javascript based
 * Light Source Engine apps, other than JSX, babel transpilation may not be needed.
 *
 * @ignore
 */

const { transformAsync } = babel
const transformOptions = {
  babelrc: false,
  configFile: false,
  compact: false,
  minified: false,
  comments: false,
  cloneInputAst: false,
  sourceType: 'module',
  parserOpts: {
    sourceType: 'module'
  },
  presets: [
    ['@babel/preset-env', { targets: { node: '14' }, shippedProposals: true, loose: true, modules: false }],
    '@babel/preset-react'
  ]
}

export const transformSource = async (source, context, defaultTransformSource) => {
  if (isJsx(context.url)) {
    const { code } = await transformAsync(source, transformOptions)

    source = code
  }

  return defaultTransformSource(source, context, defaultTransformSource)
}

export { getFormatJsx as getFormat }
