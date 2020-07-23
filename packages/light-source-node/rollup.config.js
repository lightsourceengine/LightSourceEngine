/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { onwarn, minify, nodeEnv, inlineObjectAssign } from '../rollup/plugins'
import commonjs from '@rollup/plugin-commonjs'
import nodeResolve from '@rollup/plugin-node-resolve'
import autoExternal from 'rollup-plugin-auto-external'

export default [
  // react is bundled and installed as an ls-node global library so users can use their own react version.
  {
    input: require.resolve('react'),
    onwarn,
    output: {
      exports: "default",
      format: 'cjs',
      file: 'dist/cjs/react.standalone.js'
    },
    plugins: [
      nodeEnv(),
      commonjs({
        ignoreGlobal: true
      }),
      inlineObjectAssign(),
      minify()
    ]
  },
  // bindings should be bundled with light-source; however, rollup treeshake removes a property getter that has a
  // side effect (dummy.stack). When the property is removed, bindings cannot locate the .node file on the file
  // system. Turning off treeshaking fixes the issue, but disabling treeshaking is undesirable for light-source. The
  // solution is to isolate bindings to fix the issue. As a result, bindings will be installed as a global ls-node
  // module.
  {
    input: 'lib/bindings-standalone',
    onwarn,
    output: {
      exports: "default",
      format: 'cjs',
      file: 'dist/cjs/bindings.standalone.js'
    },
    treeshake: false,
    plugins: [
      nodeEnv(),
      autoExternal(),
      nodeResolve(),
      commonjs({
        ignoreGlobal: true,
        ignore: true // allow dynamic requires
      }),
      minify()
    ]
  }
]
