/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { onwarn, beautify, replaceObjectAssign } from '../rollup/plugins'
import autoExternal from 'rollup-plugin-auto-external'

export default [
  {
    input: require.resolve('react/cjs/react.production.min.js'),
    onwarn,
    output: {
      format: 'cjs',
      file: 'dist/react.standalone.cjs'
    },
    plugins: [
      replaceObjectAssign()
    ]
  },
  {
    input: 'lib/node-path-loader.mjs',
    onwarn,
    output: {
      format: 'esm',
      file: 'dist/node-path-loader.mjs'
    },
    plugins: [
      autoExternal(),
      beautify()
    ]
  }
]
