/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { onwarn, beautify } from '../rollup/plugins'
import autoExternal from 'rollup-plugin-auto-external'

const standaloneLoader = (filename) => ({
  input: `src/${filename}`,
  onwarn,
  external: [ '@babel/core' ],
  output: {
    format: 'esm',
      file: `dist/${filename}`
  },
  plugins: [
    autoExternal(),
    beautify()
  ]
})

export default [
  standaloneLoader('builtin.mjs'),
  standaloneLoader('babel.mjs'),
  standaloneLoader('babel-jsx.mjs')
]
