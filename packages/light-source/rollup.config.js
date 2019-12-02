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
      file: 'dist/cjs/light-source.js'
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
      file: 'dist/esm/light-source.mjs'
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
      file: 'dist/cjs/light-source.standalone.js',
      preferConst: true
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelrc(),
      minify({
        reserved: [
          // light-source-react relies on function.name to be preserved for these classes
          'Scene',
          'BoxSceneNode',
          'ImageSceneNode',
          'RootSceneNode',
          'TextSceneNode'
        ]
      })
    ]
  }
]
