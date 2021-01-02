/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
    format: 'esm',
    file: standalone ? 'dist/lse-react.standalone.mjs' : 'dist/lse-react.mjs',
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

// If require() loads react and import loads react, two react instances will be in memory. React exports a cjs
// jsx-runtime that require()'s react. Convert the cjs version to mjs. The mjs version will be exported by @lse/react.
// If react exports an mjs version of jsx-runtime in the future, this rollup can be removed.
const reactJsxRuntime = (jsxRuntimeSource) => ({
  input: require.resolve(jsxRuntimeSource),
  onwarn,
  external: 'react',
  output: {
    format: 'esm',
    file: 'dist/jsx-runtime.mjs',
    preferConst: true,
    // Note: commonjs() plugin does a similar thing, but generates more code and does not handle named exports.
    intro: 'import React from "react"; const exports = {}, cache = { "react": React }, require = (id) => cache[id]',
    outro: 'export default exports; export const { ' + Object.keys(require(jsxRuntimeSource)).join(', ') + ' } = exports'
  }
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
    format: 'esm',
    file: 'dist/reconciler.mjs',
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
