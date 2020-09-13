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
import { onwarn, nodeEnv, replaceObjectAssign } from '../rollup/plugins'

// The react reconciler does not have a defined lifecycle nor does the reconciler provide an API for
// shutdown. The reconciler may hold timers through setTimeout and SchedulerMessageChannel, preventing node
// from shutting down. I was able to hijack global functions, giving me access to the components holding
// timers. From there, I expose a reconciler shutdown. This is a horrible hack, but the solution avoids forking
// the reconciler.

const lightSourceReconciler = (input, format) => ({
  input,
  onwarn,
  output: {
    format: format,
    file: `dist/index${format === 'cjs' ? '.cjs' : '.mjs'}`,
    preferConst: true
  },
  external: ['worker_threads', 'react', 'object-assign'],
  plugins: [
    autoExternal({
      dependencies: false
    }),
    replaceObjectAssign(),
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
    nodeResolve()
  ]
})

export default [
  lightSourceReconciler('lib/light-source-reconciler.js', 'esm')
]
