/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import { beautify, onwarn, babelrc } from '../rollup/plugins'
import { readdirSync } from 'fs'

const convertReactJsx = (file) => ({
  input: `src/${file}`,
  onwarn,
  external: ['@lse/core', '@lse/react', 'react'],
  output: {
    format: 'esm',
    file: `dist/${file.slice(0, -1)}`
  },
  plugins: [
    autoExternal(),
    babelrc({ extensions: ['.mjsx'] }),
    beautify()
  ]
})

export default readdirSync('src').filter(f => f.endsWith('.mjsx')).map(file => convertReactJsx(file))
