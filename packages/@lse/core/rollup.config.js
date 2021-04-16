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
import resolve from '@rollup/plugin-node-resolve'
import copy from 'rollup-plugin-copy'
import { beautify, onwarn, minify, inlineModule, getPublishingVersion } from '../../rollup/plugins.js'
import replace from 'rollup-plugin-re'

const publishingVersion = getPublishingVersion()
const lseVersion = () => replace({
  replaces: {
    $LSE_VERSION: publishingVersion
  }
})

const inlineBindings = () => inlineModule({
  bindings: 'export default {}'
})

const lightSourceNpm = (input) => (
  {
    input,
    onwarn,
    output: {
      format: 'cjs',
      file: 'dist/lse-core.cjs',
      preferConst: true
    },
    plugins: [
      autoExternal(),
      resolve(),
      lseVersion(),
      beautify(),
      copy({ targets: [{ src: 'src/font/*.woff', dest: 'dist' }] })
    ]
  }
)

const lightSourceStandalone = (input) => ({
  input,
  onwarn,
  output: {
    format: 'cjs',
    file: 'dist/lse-core.standalone.cjs',
    preferConst: true
  },
  plugins: [
    autoExternal({
      dependencies: false
    }),
    inlineBindings(),
    resolve(),
    lseVersion(),
    minify({
      reserved: [
        // @lse/react relies on function.name to be preserved for these classes
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
  lightSourceNpm('src/exports.mjs'),
  lightSourceStandalone('src/exports.mjs')
]
