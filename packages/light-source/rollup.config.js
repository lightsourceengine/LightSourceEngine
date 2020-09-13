/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import resolve from '@rollup/plugin-node-resolve'
import replace from 'rollup-plugin-re'
import { beautify, onwarn, minify, babelrc, inlineModule, noop } from '../rollup/plugins.js'

const input = 'src/exports.js'
const formatToExtension = format => format === 'esm' ? '.mjs' : 'cjs'
const inlineBindings = () => inlineModule({
  bindings: 'export default {}'
})
const setLsBindingsEnv = (value) => replace({
  sourceMap: false,
  replaces: {
    'process.env.LS_BINDINGS': JSON.stringify(value)
  }
})

const lightSourceNpm = (input, format) => (
  {
    input,
    onwarn,
    output: {
      format,
      file: `dist/light-source${formatToExtension(format)}`
    },
    plugins: [
      autoExternal(),
      setLsBindingsEnv('bindings'),
      resolve(),
      format === 'esm' ? noop() : babelrc(),
      beautify()
    ]
  }
)

const lightSourceStandalone = (input, format) => ({
  input,
  onwarn,
  output: {
    format,
    file: `dist/light-source.standalone${formatToExtension(format)}`,
    preferConst: true
  },
  plugins: [
    autoExternal({
      dependencies: false
    }),
    inlineBindings(),
    setLsBindingsEnv('custom-bindings'),
    resolve(),
    format === 'esm' ? noop() : babelrc(),
    minify({
      reserved: [
        // light-source-react relies on function.name to be preserved for these classes
        'Scene',
        'BoxSceneNode',
        'ImageSceneNode',
        'RootSceneNode',
        'TextSceneNode'
      ]
    })
  ]
})

export default [
  lightSourceNpm(input, 'cjs'),
  lightSourceNpm(input, 'esm'),
  lightSourceStandalone(input, 'cjs'),
  lightSourceStandalone(input, 'esm')
]
