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

import { fileURLToPath, pathToFileURL } from 'url'
import { dirname, join, resolve as resolvePath } from 'path'

const builtins = (() => {
  const { LSE_ENV, LSE_PATH } = process.env

  if (LSE_ENV === 'runtime') {
    const toUrl = (...parts) => pathToFileURL(resolvePath(LSE_PATH, ...parts)).href

    return {
      react: { url: toUrl('react', 'index.cjs') },
      'react/jsx-runtime': { url: toUrl('react', 'jsx-runtime.cjs') },
      'react/jsx-dev-runtime': { url: toUrl('react', 'jsx-dev-runtime.cjs') },
      '@lse/core': { url: toUrl('@lse', 'core', 'index.cjs') },
      '@lse/react': { url: toUrl('@lse', 'react', 'index.cjs') },
      '@lse/react/jsx-runtime': { url: toUrl('@lse', 'react', 'jsx-runtime.cjs') }
    }
  }

  return {}
})()

export const resolve = async (specifier, context, defaultResolve) => {
  if (specifier.endsWith('_mocha')) {
    // Find mocha's index.js in node_modules.
    const result = await defaultResolve('mocha', context)

    // Find the _mocha bin file relative to index.js. In order to load the file later, getFormat will
    // mark _mocha as a commonjs file.
    return {
      url: pathToFileURL(join(dirname(fileURLToPath(result.url)), 'bin', '_mocha')).href
    }
  }

  // mimic the behavior of require. search locally, then load from builtin folder
  try {
    return defaultResolve(specifier, context)
  } catch (e) {
    if (builtins[specifier]) {
      return builtins[specifier]
    } else {
      throw e
    }
  }
}
