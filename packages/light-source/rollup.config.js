/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import resolve from 'rollup-plugin-node-resolve'
import { beautify, onwarn, minify, babelrc } from '../rollup/plugins'

const input = 'src/exports.js'

export default [
  {
    input,
    onwarn,
    output: {
      format: 'cjs',
      file: 'dist/cjs/index.js'
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelrc(),
      beautify()
    ]
  },
  {
    input,
    onwarn,
    output: {
      format: 'esm',
      file: 'dist/esm/index.mjs'
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelrc(),
      beautify()
    ]
  },
  {
    input,
    onwarn,
    output: {
      format: 'cjs',
      file: 'build/standalone/cjs/light-source.min.js',
      preferConst: true
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelrc(),
      minify()
    ]
  }
]
