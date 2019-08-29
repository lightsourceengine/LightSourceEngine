/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import commonjs from 'rollup-plugin-commonjs'
import resolve from 'rollup-plugin-node-resolve'
import replace from 'rollup-plugin-re'
import {
  beautify,
  babelPreserveImports,
  onwarn,
  getNamedExports,
  minify,
  inlineModule
} from '../../scripts/rollup-common'

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
    external: ['react'],
    output: {
      format: 'cjs',
      file: 'dist/cjs/react-light-source.min.js',
      preferConst: true
    },
    plugins: [
      autoExternal({ peerDependencies: false }),
      inlineModule({
        'object-assign': 'export default Object.assign'
      }),
      replace({
        replaces: {
          'process.env.NODE_ENV': JSON.stringify('production')
        }
      }),
      resolve(),
      babelPreserveImports({
        babelConfigPath: __dirname
      }),
      commonjs({
        include: ['/**/node_modules/**'],
        namedExports: {
          ...getNamedExports(['react-reconciler', 'scheduler'])
        }
      }),
      minify()
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
  }
]
