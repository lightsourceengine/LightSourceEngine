/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import { terser } from 'rollup-plugin-terser'

const template = {
  input: 'index.js',
  onwarn (warning, warn) {
    warn(warning)
    throw Error(warning.message)
  },
  external: ['rollup-plugin-terser', 'rollup-plugin-babel'],
  plugins: [
    autoExternal(),
    terser({
      compress: false,
      mangle: false,
      output: {
        ecma: 8,
        quote_style: 1,
        semicolons: false,
        beautify: true
      }
    })
  ]
}

export default [
  {
    output: {
      format: 'cjs',
      file: 'dist/cjs/index.js'
    },
    ...template
  },
  {
    output: {
      format: 'esm',
      file: 'dist/esm/index.mjs'
    },
    ...template
  }
]