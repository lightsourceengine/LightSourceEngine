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

const { LSE_ENV, LSE_PATH } = process.env

const builtins = (LSE_ENV === 'lse-node')
  ? {
      react: { url: pathToFileURL(resolvePath(LSE_PATH, 'react', 'index.cjs')).toString() },
      '@lse/core': { url: pathToFileURL(resolvePath(LSE_PATH, '@lse', 'core', 'index.mjs')).toString() },
      '@lse/react': { url: pathToFileURL(resolvePath(LSE_PATH, '@lse', 'react', 'index.mjs')).toString() },
      '@lse/react/jsx-runtime': { url: pathToFileURL(resolvePath(LSE_PATH, '@lse', 'react', 'jsx-runtime.cjs')).toString() },
      '@lse/react/reconciler': { url: pathToFileURL(resolvePath(LSE_PATH, '@lse', 'react', 'reconciler.mjs')).toString() }
    }
  : {}

export const resolve = async (specifier, context, defaultResolve) => {
  if (specifier.endsWith('_mocha')) {
    // Find mocha's index.js in node_modules.
    const result = await defaultResolve('mocha', context)

    // Find the _mocha bin file relative to index.js. In order to load the file later, getFormat will
    // mark _mocha as a commonjs file.
    return {
      url: pathToFileURL(join(dirname(fileURLToPath(result.url)), 'bin', '_mocha')).toString()
    }
  }

  return builtins[specifier] ?? defaultResolve(specifier, context)
}
