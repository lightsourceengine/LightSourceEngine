/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { join, resolve as resolvePath, dirname } from 'path'

const globalPath = process.env.NODE_PATH ?? join(dirname(process.execPath), '..', 'lib', 'node_modules')

// TODO: support the use case of the user supplying their own local version of react

const globalModules = {
  react: resolvePath(globalPath, 'react/index.cjs'),
  'light-source': resolvePath(globalPath, 'light-source/index.mjs'),
  'light-source-react': resolvePath(globalPath, 'light-source-react/index.mjs')
}

export async function resolve (specifier, context, defaultResolver) {
  console.log('resolve: ' + specifier)
  console.log('context: ' + context)
  console.log(globalModules[specifier] ?? specifier)
  return defaultResolver(globalModules[specifier] ?? specifier, context)
}
