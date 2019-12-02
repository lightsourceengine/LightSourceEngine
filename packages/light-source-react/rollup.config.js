/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import commonjs from 'rollup-plugin-commonjs'
import resolve from 'rollup-plugin-node-resolve'
import { beautify, onwarn, minify, babelrc, nodeEnv } from '../rollup/plugins'

const input = 'src/exports.js'
const cjs = () => commonjs({
  ignoreGlobal: true
})

// Note: light-source-reconciler is included in all of these files intentionally to reduce the complexity for library users.

export default [
  {
    input,
    onwarn,
    external: ['worker_threads'],
    output: {
      format: 'cjs',
      file: 'dist/cjs/light-source-react.js'
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
    external: ['worker_threads'],
    output: {
      format: 'esm',
      file: 'dist/esm/light-source-react.mjs'
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
    external: ['react', 'worker_threads'],
    output: {
      format: 'cjs',
      file: 'dist/cjs/light-source-react.standalone.js',
      preferConst: true
    },
    plugins: [
      autoExternal({ peerDependencies: false }),
      nodeEnv(),
      resolve(),
      babelrc(),
      cjs(),
      minify()
    ]
  }
]
