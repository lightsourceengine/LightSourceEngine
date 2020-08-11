/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import commonjs from '@rollup/plugin-commonjs'
import resolve from '@rollup/plugin-node-resolve'
import { beautify, onwarn, babelrc } from '../rollup/plugins'

const cjs = () => commonjs({
  ignoreGlobal: true
})

export default [
  {
    input: 'src/img-objectfit.js',
    onwarn,
    external: ['light-source', 'light-source-react', 'react'],
    output: {
      format: 'cjs',
      file: 'dist/cjs/img-objectfit.js'
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelrc(),
      cjs(),
      beautify()
    ]
  },
  {
    input: 'src/img-objectfit.js',
    onwarn,
    external: ['light-source', 'light-source-react', 'react'],
    output: {
      format: 'esm',
      file: 'dist/esm/img-objectfit.mjs'
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelrc(),
      cjs(),
      beautify()
    ]
  }
]
