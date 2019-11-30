/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import commonjs from 'rollup-plugin-commonjs'
import resolve from 'rollup-plugin-node-resolve'
import { beautify, onwarn, minify, babelrc, nodeEnv, inlineObjectAssign } from '../rollup/plugins'

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
      file: 'dist/cjs/index.js'
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
      file: 'dist/esm/index.mjs'
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
      file: 'build/standalone/cjs/light-source-react.min.js',
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
  },
  {
    input: require.resolve('react'),
    onwarn,
    output: {
      format: 'cjs',
      file: 'build/standalone/cjs/react.min.js'
    },
    plugins: [
      nodeEnv(),
      cjs(),
      inlineObjectAssign(),
      minify()
    ]
  }
]
