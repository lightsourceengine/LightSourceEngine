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

const reactStandalone = (reactSource, file) => ({
  input: require.resolve(reactSource),
  onwarn,
  output: {
    format: 'cjs',
    file: `dist/${file}.cjs`,
    preferConst: true
  },
  plugins: [
    replaceObjectAssign()
  ]
})

export default [
  reactStandalone('react/cjs/react.production.min.js', 'index'),
  reactStandalone('react/cjs/react-jsx-runtime.production.min.js', 'jsx-runtime'),
  reactStandalone('react/cjs/react-jsx-dev-runtime.production.min.js', 'jsx-dev-runtime')
]