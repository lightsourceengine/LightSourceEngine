/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import resolve from '@rollup/plugin-node-resolve'
import commonjs from '@rollup/plugin-commonjs'
import { beautify, onwarn, minify, babelrc, replaceObjectAssign, noop } from '../rollup/plugins'

const input = 'src/exports.js'
const isCjs = (format) => format === 'cjs'
const formatToExtension = format => isCjs(format) ? '.cjs' : '.mjs'
const cjs = () => commonjs({
  ignoreGlobal: true
})

const lightSourceReactNpm = (input, format) => ({
  input,
  onwarn,
  output: {
    format,
    file: `dist/light-source-react${formatToExtension(format)}`
  },
  plugins: [
    autoExternal(),
    resolve(),
    isCjs(format) ? babelrc() : noop(),
    isCjs(format) ? cjs() : noop(),
    beautify()
  ]
})
const lightSourceReactStandalone = (input, format) => ({
  input,
  onwarn,
  external: ['light-source', 'react', 'worker_threads'],
  output: {
    format,
    file: `dist/light-source-react.standalone${formatToExtension(format)}`,
    preferConst: true
  },
  plugins: [
    autoExternal({ dependencies: false, peerDependencies: false }),
    resolve(),
    isCjs(format) ? babelrc() : noop(),
    isCjs(format) ? cjs() : noop(),
    minify()
  ]
})

export default [
  lightSourceReactNpm(input, 'cjs'),
  lightSourceReactNpm(input, 'esm'),
  lightSourceReactStandalone(input, 'cjs'),
  lightSourceReactStandalone(input, 'esm'),
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
  }
]
