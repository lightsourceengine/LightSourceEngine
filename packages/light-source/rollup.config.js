/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import resolve from '@rollup/plugin-node-resolve'
import replace from 'rollup-plugin-re'
import { beautify, onwarn, minify, babelrc, inlineModule } from '../rollup/plugins.js'

const input = 'src/exports.js'

export default [
  {
    input,
    onwarn,
    output: {
      format: 'cjs',
      file: 'dist/light-source.cjs'
    },
    plugins: [
      autoExternal(),
      replace({
        sourceMap: false,
        replaces: {
          'process.env.LS_BINDINGS': JSON.stringify('bindings')
        }
      }),
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
      file: 'dist/light-source.standalone.cjs',
      preferConst: true
    },
    plugins: [
      autoExternal({
        dependencies: false
      }),
      inlineModule({
        bindings: 'export default {}'
      }),
      replace({
        sourceMap: false,
        replaces: {
          'process.env.LS_BINDINGS': JSON.stringify('custom-bindings')
        }
      }),
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
