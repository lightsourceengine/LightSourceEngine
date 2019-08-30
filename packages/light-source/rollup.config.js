/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import commonjs from 'rollup-plugin-commonjs'
import resolve from 'rollup-plugin-node-resolve'
import { beautify, babelPreserveImports, onwarn, minify, inlineModule } from 'light-source-rollup'

const input = 'src/exports.js'

export default [
  {
    input,
    onwarn,
    output: {
      format: 'cjs',
      file: 'dist/cjs/index.js'
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelPreserveImports({
        babelConfigPath: __dirname
      }),
      commonjs({
        exclude: ['/**/node_modules/**']
      }),
      beautify()
    ]
  },
  {
    input,
    onwarn,
    output: {
      format: 'esm',
      file: 'dist/esm/index.mjs'
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelPreserveImports({
        babelConfigPath: __dirname
      }),
      beautify()
    ]
  },
  {
    input,
    onwarn,
    output: {
      format: 'cjs',
      file: 'build/standalone/cjs/light-source.min.js',
      preferConst: true
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelPreserveImports({
        babelConfigPath: __dirname
      }),
      commonjs({
        exclude: ['/**/node_modules/**']
      }),
      minify()
    ]
  },
  {
    input: 'bindings-inline',
    onwarn,
    output: {
      format: 'cjs',
      file: 'build/standalone/cjs/bindings.min.js'
    },
    plugins: [
      inlineModule({
        'bindings-inline': 'import bindings from \'bindings\'; export default bindings'
      }),
      autoExternal({ dependencies: false }),
      resolve(),
      commonjs({
        include: ['/**/node_modules/**'],
        ignore: true
      }),
      minify()
    ]
  }
]
