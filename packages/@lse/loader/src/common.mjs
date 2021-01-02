/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { fileURLToPath, pathToFileURL } from 'url'
import { dirname, join, resolve as resolvePath } from 'path'

const { LSE_ENV, LSE_PATH } = process.env

const builtins = (LSE_ENV === 'lse-node') ? {
  react: { url: pathToFileURL(resolvePath(LSE_PATH, 'react', 'index.cjs')).toString() },
  '@lse/core': { url: pathToFileURL(resolvePath(LSE_PATH, '@lse', 'core', 'index.mjs')).toString() },
  '@lse/react': { url: pathToFileURL(resolvePath(LSE_PATH, '@lse', 'react', 'index.mjs')).toString() },
  '@lse/react/jsx-runtime': { url: pathToFileURL(resolvePath(LSE_PATH, '@lse', 'react', 'jsx-runtime.mjs')).toString() },
  '@lse/react/reconciler': { url: pathToFileURL(resolvePath(LSE_PATH, '@lse', 'react', 'reconciler.mjs')).toString() }
} : {}

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
