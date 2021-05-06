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

import { onwarn, replaceObjectAssign } from '../rollup/plugins'
import copy from 'rollup-plugin-copy'
import replace from 'rollup-plugin-re'

const bindings = () => (
  {
    treeshake: false,
    input: require.resolve('bindings/bindings.js'),
    onwarn,
    output: {
      format: 'cjs',
      file: 'dist/bindings.cjs',
      preferConst: true
    },
    plugins: [
      replace({
        replaces: {
          'fileURLToPath = require(\'file-uri-to-path\')': '{ fileURLToPath } = require(\'url\')'
        }
      }),
      copy({
        targets: [{src: 'bindings-package.json' , dest: 'dist' }],
        copyOnce: true
      })
    ]
  }
)

const reactStandalone = (reactSource, file) => ({
  input: require.resolve(reactSource),
  onwarn,
  output: {
    format: 'cjs',
    file: `dist/${file}.cjs`,
    preferConst: true
  },
  plugins: [
    replaceObjectAssign(),
    copy({
      targets: [{src: 'react-package.json' , dest: 'dist' }],
      copyOnce: true
    })
  ]
})

export default [
  bindings(),
  reactStandalone('react/cjs/react.production.min.js', 'react'),
  reactStandalone('react/cjs/react-jsx-runtime.production.min.js', 'jsx-runtime'),
  reactStandalone('react/cjs/react-jsx-dev-runtime.production.min.js', 'jsx-dev-runtime')
]
