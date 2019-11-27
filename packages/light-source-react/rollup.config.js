/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { readFileSync } from 'fs'
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
} from 'light-source-rollup'

const input = 'src/exports.js'

const removeSchedulerInterop = () => ({
  name: 'removeSchedulerInterop',
  load (id) {
    return id.includes('scheduler-interop.js') ? '' : null
  }
}
)

const readSchedulerInterop = () => readFileSync('src/scheduler-interop.js')

export default [
  {
    input,
    onwarn,
    external: ['worker_threads'],
    output: {
      format: 'cjs',
      file: 'dist/cjs/index.js',
      intro: readSchedulerInterop
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelPreserveImports({
        babelConfigPath: __dirname
      }),
      removeSchedulerInterop(),
      commonjs({
        exclude: ['/**/node_modules/**'],
        ignoreGlobal: true
      }),
      beautify()
    ]
  },
  {
    input,
    onwarn,
    external: ['worker_threads'],
    output: {
      format: 'esm',
      file: 'dist/esm/index.mjs',
      intro: readSchedulerInterop
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelPreserveImports({
        babelConfigPath: __dirname
      }),
      removeSchedulerInterop(),
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
        ignoreGlobal: true,
        namedExports: {
          ...getNamedExports(['react-reconciler', 'scheduler'])
        }
      }),
      minify()
    ]
  },
  {
    input: '../../node_modules/react/cjs/react.production.min.js',
    onwarn,
    output: {
      format: 'cjs',
      file: 'build/standalone/cjs/react.min.js'
    },
    plugins: [
      replace({
        replaces: {
          'require(\'object-assign\')': 'Object.assign',
          'require("object-assign")': 'Object.assign'
        }
      })
    ]
  }
]
