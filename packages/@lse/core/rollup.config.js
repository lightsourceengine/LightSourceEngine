/*
 * Copyright (c) 2019 Daniel Anderson.
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import autoExternal from 'rollup-plugin-auto-external'
import resolve from '@rollup/plugin-node-resolve'
import copy from 'rollup-plugin-copy'
import { beautify, onwarn, minify, inlineModule, getPublishingVersion } from '../../rollup/plugins.js'

const intro = `const LIGHT_SOURCE_VERSION = '${getPublishingVersion()}'; const INTRINSIC_FONT_DIR = 'font';`

const inlineBindings = () => inlineModule({
  bindings: 'export default {}'
})

const lightSourceNpm = (input) => (
  {
    input,
    onwarn,
    output: {
      format: 'esm',
      file: 'dist/light-source.mjs',
      preferConst: true,
      intro
    },
    plugins: [
      autoExternal(),
      resolve(),
      beautify(),
      copy({ targets: [{ src: 'src/font/*', dest: 'dist/font' }] })
    ]
  }
)

const lightSourceStandalone = (input) => ({
  input,
  onwarn,
  output: {
    format: 'esm',
    file: 'dist/light-source.standalone.mjs',
    preferConst: true,
    intro
  },
  plugins: [
    autoExternal({
      dependencies: false
    }),
    inlineBindings(),
    resolve(),
    minify({
      reserved: [
        // light-source-react relies on function.name to be preserved for these classes
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
