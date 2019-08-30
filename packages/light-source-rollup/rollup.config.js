/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import { terser } from 'rollup-plugin-terser'

const input = 'index.js'

const external = ['rollup-plugin-terser', 'rollup-plugin-babel']

const onwarn = (warning, warn) => {
  warn(warning)
  throw Error(warning.message)
}

const beautify = () => terser({
  compress: false,
  mangle: false,
  output: {
    ecma: 8,
    quote_style: 1,
    semicolons: false,
    beautify: true
  }
})

export default [
  {
    input,
    onwarn,
    external,
    output: {
      format: 'cjs',
      file: 'dist/cjs/index.js'
    },
    plugins: [
      autoExternal(),
      beautify()
    ]
  },
  {
    input,
    external,
    onwarn,
    output: {
      format: 'esm',
      file: 'dist/esm/index.mjs'
    },
    plugins: [
      autoExternal(),
      beautify()
    ]
  }
]
