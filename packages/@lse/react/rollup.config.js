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
import nodeResolve from '@rollup/plugin-node-resolve'
import {
  beautify,
  onwarn,
  minify,
  replaceObjectAssign,
  getPublishingVersion
} from '../../rollup/plugins'
import replace from 'rollup-plugin-re'

const lightSourceReact = ({ input, standalone }) => ({
  input,
  onwarn,
  external: ['@lse/core'],
  output: {
    format: 'cjs',
    file: standalone ? 'dist/lse-react.standalone.cjs' : 'dist/lse-react.cjs',
    preferConst: true
  },
  plugins: [
    autoExternal(),
    nodeResolve(),
    standalone ? minify() : beautify(),
    replace({
      replaces: {
        $LSE_REACT_VERSION: getPublishingVersion()
      }
    })
  ]
})

// babel imports react/jsx-runtime with no extension. mjs modules cannot find the jsx-runtime due
// to how the react package.json is set up. babel users will need to set the import source and use
// @lse/react/jsx-runtime to transpile
const reactJsxRuntime = (jsxRuntimeSource) => ({
  input: require.resolve(jsxRuntimeSource),
  onwarn,
  external: 'react',
  output: {
    format: 'cjs',
    file: 'dist/jsx-runtime.cjs'
  },
  plugins: [
    replaceObjectAssign()
  ]
})

export default [
  lightSourceReact({ input: 'src/exports.mjs' }),
  lightSourceReact({ input: 'src/exports.mjs', standalone: true }),
  reactJsxRuntime('react/cjs/react-jsx-runtime.production.min.js')
]
