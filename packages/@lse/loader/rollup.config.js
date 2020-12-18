/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { onwarn, beautify } from '../../rollup/plugins'
import autoExternal from 'rollup-plugin-auto-external'

const standaloneLoader = (stem) => ({
  input: `src/${stem}.mjs`,
  onwarn,
  external: ['@babel/core'],
  output: {
    format: 'esm',
    file: `dist/${stem}.standalone.mjs`
  },
  plugins: [
    autoExternal(),
    beautify()
  ]
})

export default [
  standaloneLoader('builtin'),
  standaloneLoader('babel'),
  standaloneLoader('babel-jsx'),
  standaloneLoader('babel-mocha')
]
