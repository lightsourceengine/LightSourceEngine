/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import nodeResolve from '@rollup/plugin-node-resolve'
import inject from '@rollup/plugin-inject'
import commonjs from '@rollup/plugin-commonjs'
import { resolve } from 'path'
import { onwarn, minify, nodeEnv, inlineObjectAssign } from '../rollup/plugins'

export default [
  {
    input: 'lib/light-source-reconciler.js',
    onwarn,
    output: {
      format: 'cjs',
      file: 'dist/cjs/index.js',
      preferConst: true
    },
    external: ['worker_threads', 'react'],
    plugins: [
      autoExternal({
        dependencies: false
      }),
      nodeEnv(),
      commonjs({
        ignoreGlobal: true
      }),
      inject({
        MessageChannel: resolve('lib/SchedulerMessageChannel.js'),
        window: resolve('lib/window-polyfill.js'),
        setTimeout: [resolve('lib/timeout-scope.js'), 'setTimeout'],
        clearTimeout: [resolve('lib/timeout-scope.js'), 'clearTimeout']
      }),
      inlineObjectAssign(),
      nodeResolve(),
      minify()
    ]
  }
]
