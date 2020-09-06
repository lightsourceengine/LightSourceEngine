/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import resolve from '@rollup/plugin-node-resolve'
import commonjs from '@rollup/plugin-commonjs'
import { beautify, onwarn, minify, babelrc, nodeEnv } from '../rollup/plugins'

const input = 'src/exports.js'
const cjs = () => commonjs({
  ignoreGlobal: true
})

export default [
  {
    input,
    onwarn,
    output: {
      format: 'cjs',
      file: 'dist/light-source-react.cjs'
    },
    plugins: [
      autoExternal(),
      nodeEnv(),
      resolve(),
      babelrc(),
      cjs(),
      beautify()
    ]
  },
  {
    input,
    onwarn,
    external: ['light-source', 'react', 'worker_threads'],
    output: {
      format: 'cjs',
      file: 'dist/light-source-react.standalone.cjs',
      preferConst: true
    },
    plugins: [
      autoExternal({ dependencies: false, peerDependencies: false }),
      nodeEnv(),
      resolve(),
      babelrc(),
      cjs(),
      minify()
    ]
  }
]
