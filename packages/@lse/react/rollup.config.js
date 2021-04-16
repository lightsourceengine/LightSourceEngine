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
import {
  beautify,
  onwarn,
  minify,
  replaceObjectAssign,
  getPublishingVersion,
  nodeEnv
} from '../../rollup/plugins'
import commonjs from '@rollup/plugin-commonjs'
import inject from '@rollup/plugin-inject'
import replace from 'rollup-plugin-re'

const lightSourceReact = ({ input, standalone }) => ({
  input,
  onwarn,
  external: ['@lse/core', '@lse/react/reconciler'],
  output: {
    format: 'cjs',
    file: standalone ? 'dist/lse-react.standalone.cjs' : 'dist/lse-react.cjs',
    preferConst: true
  },
  plugins: [
    autoExternal(),
    nodeResolve(),
    standalone ? minify() : beautify(),
    replace({
      replaces: {
        $LSE_REACT_VERSION: getPublishingVersion()
      }
    })
  ]
})

const reactStandalone = (reactSource) => ({
  input: require.resolve(reactSource),
  onwarn,
  output: {
    format: 'cjs',
    file: 'dist/react.standalone.cjs',
    preferConst: true
  },
  plugins: [
    replaceObjectAssign()
  ]
})

// babel imports react/jsx-runtime with no extension. mjs modules cannot find the jsx-runtime due
// to how the react package.json is set up. babel users will need to set the import source and use
// @lse/react/jsx-runtime to transpile
const reactJsxRuntime = (jsxRuntimeSource) => ({
  input: require.resolve(jsxRuntimeSource),
  onwarn,
  external: 'react',
  output: {
    format: 'cjs',
    file: 'dist/jsx-runtime.cjs'
  },
  plugins: [
    replaceObjectAssign()
  ]
})

// The react reconciler does not have a defined lifecycle nor does the reconciler provide an API for
// shutdown. The reconciler may hold timers through setTimeout and SchedulerMessageChannel, preventing node
// from shutting down. I was able to hijack global functions, giving me access to the components holding
// timers. From there, I expose a reconciler shutdown. This is a horrible hack, but the solution avoids forking
// the reconciler.
const lightSourceReconciler = (input) => ({
  input,
  onwarn,
  output: {
    format: 'cjs',
    file: 'dist/reconciler.cjs',
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
      MessageChannel: resolve('src/reconciler/SchedulerMessageChannel.js'),
      window: resolve('src/reconciler/window-polyfill.js'),
      performance: ['perf_hooks', 'performance'],
      setTimeout: [resolve('src/reconciler/timeout-scope.js'), 'setTimeout'],
      clearTimeout: [resolve('src/reconciler/timeout-scope.js'), 'clearTimeout']
    }),
    nodeResolve(),
    minify()
  ]
})

export default [
  lightSourceReconciler('src/reconciler/light-source-reconciler.js'),
  lightSourceReact({ input: 'src/exports.js' }),
  lightSourceReact({ input: 'src/exports.js', standalone: true }),
  reactJsxRuntime('react/cjs/react-jsx-runtime.production.min.js'),
  reactStandalone('react/cjs/react.production.min.js')
]
