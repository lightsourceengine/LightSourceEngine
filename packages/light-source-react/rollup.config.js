/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import resolve from '@rollup/plugin-node-resolve'
import { beautify, onwarn, minify, replaceObjectAssign } from '../rollup/plugins'

const lightSourceReactNpm = (input) => ({
  input,
  onwarn,
  output: {
    format: 'esm',
    file: 'dist/light-source-react.mjs',
    preferConst: true
  },
  plugins: [
    autoExternal(),
    resolve(),
    beautify()
  ]
})

const lightSourceReactStandalone = (input) => ({
  input,
  onwarn,
  external: ['light-source', 'react', 'worker_threads'],
  output: {
    format: 'esm',
    file: 'dist/light-source-react.standalone.mjs',
    preferConst: true
  },
  plugins: [
    autoExternal({ dependencies: false, peerDependencies: false }),
    resolve(),
    minify()
  ]
})

const reactStandalone = () => ({
  input: require.resolve('react/cjs/react.production.min.js'),
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

export default [
  lightSourceReactNpm('src/exports.js'),
  lightSourceReactStandalone('src/exports.js'),
  reactStandalone()
]
