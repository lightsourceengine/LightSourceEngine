/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import autoExternal from 'rollup-plugin-auto-external'
import { beautify, onwarn, babelrc } from '../rollup/plugins'
import { readdirSync } from 'fs'

const convertReactJsx = (file) => ({
  input: `src/${file}`,
  onwarn,
  external: ['@lse/core', '@lse/react', '@lse/react/jsx-runtime', 'react'],
  output: {
    format: 'esm',
    file: `lib/${file.slice(0, -1)}`
  },
  plugins: [
    autoExternal(),
    babelrc({ extensions: ['.mjsx'] }),
    beautify()
  ]
})

export default readdirSync('src').filter(f => f.endsWith('.mjsx')).map(file => convertReactJsx(file))
