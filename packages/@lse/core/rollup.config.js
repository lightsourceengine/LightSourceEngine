/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
      format: 'esm',
      file: 'dist/lse-core.mjs',
      preferConst: true
    },
    plugins: [
      autoExternal(),
      resolve(),
      lseVersion(),
      beautify(),
      copy({ targets: [{ src: 'src/font/*.ttf', dest: 'dist' }] })
    ]
  }
)

const lightSourceStandalone = (input) => ({
  input,
  onwarn,
  output: {
    format: 'esm',
    file: 'dist/lse-core.standalone.mjs',
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
  lightSourceNpm('src/exports.js'),
  lightSourceStandalone('src/exports.js')
]
