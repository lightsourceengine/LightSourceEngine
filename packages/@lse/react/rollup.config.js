/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import resolve from '@rollup/plugin-node-resolve'
import { beautify, onwarn, minify, replaceObjectAssign, getPublishingVersion, getPackageJsonVersion } from '../../rollup/plugins'

const lightSourceReactVersion = JSON.stringify(getPublishingVersion())
const reactVersion = JSON.stringify(getPackageJsonVersion(require.resolve('react/package.json')))
const intro = `global.lightSourceReactVersion = ${lightSourceReactVersion}; global.reactVersion=${reactVersion}`

const lightSourceReactNpm = (input) => ({
  input,
  onwarn,
  external: ['@lse/core'],
  output: {
    format: 'esm',
    file: 'dist/lse-react.mjs',
    preferConst: true,
    intro
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
  external: ['@lse/core', 'react', 'worker_threads'],
  output: {
    format: 'esm',
    file: 'dist/lse-react.standalone.mjs',
    preferConst: true,
    intro
  },
  plugins: [
    autoExternal({ dependencies: false, peerDependencies: false }),
    resolve(),
    minify()
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

export default [
  lightSourceReactNpm('src/exports.js'),
  lightSourceReactStandalone('src/exports.js'),
  reactJsxRuntime('react/cjs/react-jsx-runtime.production.min.js'),
  reactStandalone('react/cjs/react.production.min.js')
]
