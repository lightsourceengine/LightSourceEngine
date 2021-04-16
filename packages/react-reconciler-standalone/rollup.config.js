/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import autoExternal from 'rollup-plugin-auto-external'
import nodeResolve from '@rollup/plugin-node-resolve'
import { resolve } from 'path'
import { onwarn, replaceObjectAssign, nodeEnv, beautify } from '../rollup/plugins'
import commonjs from '@rollup/plugin-commonjs'
import inject from '@rollup/plugin-inject'

// The react reconciler does not have a defined lifecycle nor does the reconciler provide an API for
// shutdown. The reconciler may hold timers through setTimeout and SchedulerMessageChannel, preventing node
// from shutting down. I was able to hijack global functions, giving me access to the components holding
// timers. From there, I expose a reconciler shutdown. This is a horrible hack, but the solution avoids forking
// the reconciler.
const lightSourceReconciler = (input) => ({
  input,
  onwarn,
  output: {
    format: 'esm',
    file: 'dist/index.mjs',
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
      MessageChannel: resolve('src/SchedulerMessageChannel.mjs'),
      window: resolve('src/window-polyfill.mjs'),
      performance: ['perf_hooks', 'performance'],
      setTimeout: [resolve('src/timeout-scope.mjs'), 'setTimeout'],
      clearTimeout: [resolve('src/timeout-scope.mjs'), 'clearTimeout']
    }),
    nodeResolve(),
    beautify()
  ]
})

export default [
  lightSourceReconciler('src/reconciler.mjs')
]
