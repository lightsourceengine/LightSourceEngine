/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import commonjs from '@rollup/plugin-commonjs'
import resolve from '@rollup/plugin-node-resolve'
import { beautify, onwarn, babelrc } from '../rollup/plugins'
import { readdirSync } from 'fs'

const cjs = () => commonjs({
  ignoreGlobal: true
})

const files = readdirSync('src').map(file => {
  return {
    input: `src/${file}`,
    onwarn,
    external: ['light-source', 'light-source-react', 'react'],
    output: {
      format: 'cjs',
      file: `dist/${file}`
    },
    plugins: [
      autoExternal(),
      resolve(),
      babelrc(),
      cjs(),
      beautify()
    ]
  }
})

export default files
