/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { join, resolve as resolvePath, dirname } from 'path'

const builtinPath = resolvePath(dirname(process.execPath), '..', 'lib', 'node_modules')
const generalModules = {
  react: resolvePath(builtinPath, 'react/index.cjs')
}
const lightSourceModules = {
  'light-source': join(builtinPath, 'light-source/index.mjs'),
  'light-source-react': join(builtinPath, 'light-source-react/index.mjs')
}

export const resolve = async (specifier, context, defaultResolver) => {
  let replacement

  if ((replacement = lightSourceModules[specifier])) {
    return defaultResolver(replacement, context)
  }

  try {
    return defaultResolver(specifier, context)
  } catch (e) {
    if ((replacement = generalModules[specifier])) {
      return defaultResolver(replacement, context)
    } else {
      throw e
    }
  }
}
