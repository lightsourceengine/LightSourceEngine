/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { inlineModule, minify, onwarn } from '../../scripts/rollup-common'
import commonjs from 'rollup-plugin-commonjs'
import resolve from 'rollup-plugin-node-resolve'
import replace from 'rollup-plugin-re'
import copy from 'rollup-plugin-copy'
import autoExternal from 'rollup-plugin-auto-external'
import { ensureDir, readJson, writeJson } from 'fs-extra'
import { join } from 'path'

const copyPackageJson = (modules) => ({
  name: 'packageJson',
  async buildEnd () {
    for (const id of modules) {
      const srcJson = await readJson(join('node_modules', id, 'package.json'))
      const dest = join('build', 'node_modules', id)

      await ensureDir(dest)
      await writeJson(join(dest, 'package.json'), {
        name: srcJson.name,
        version: srcJson.version
      }, { spaces: 2 })
    }
  }
}
)

const copyIndexJs = (options) => {
  for (const target of options.targets) {
    target.dest = join('build', 'node_modules', target.dest)
    target.rename = 'index.js'
  }

  return copy(options)
}

export default [
  {
    input: 'bindings-inline',
    onwarn,
    output: {
      format: 'cjs',
      file: 'build/node_modules/bindings/index.js'
    },
    plugins: [
      inlineModule({
        'bindings-inline': 'import bindings from \'bindings\'; export default bindings'
      }),
      autoExternal({ dependencies: false }),
      resolve(),
      commonjs({
        include: ['/**/node_modules/**'],
        ignore: true
      }),
      minify(),
      copyIndexJs({
        targets: [
          {
            src: 'node_modules/light-source/dist/cjs/light-source.min.js',
            dest: 'light-source'
          },
          {
            src: 'node_modules/react-light-source/dist/cjs/react-light-source.min.js',
            dest: 'react-light-source'
          }
        ]
      }),
      copyPackageJson([
        'bindings',
        'light-source',
        'react-light-source'
      ])
    ]
  },
  {
    input: 'node_modules/react/cjs/react.production.min.js',
    onwarn,
    output: {
      format: 'cjs',
      file: 'build/node_modules/react/index.js'
    },
    plugins: [
      replace({
        replaces: {
          'require(\'object-assign\')': 'Object.assign',
          'require("object-assign")': 'Object.assign'
        }
      }),
      copyPackageJson([
        'react'
      ])
    ]
  }
]
