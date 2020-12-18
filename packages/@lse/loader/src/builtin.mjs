/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { join, resolve as resolvePath, dirname, basename } from 'path'
import { fileURLToPath, pathToFileURL } from 'url'

/**
 * Module loader for the Light Source Engine packaged runtime.
 *
 * The loader treats 'light-source', 'light-source-react' and 'react' as builtin modules. If a developer is using
 * a packaged runtime as a development environment, the builtin modules should NOT appear in the package.json
 * dependencies (peerDependencies is ok). When a developer bundles an app targeting a packaged runtime, any
 * builtin modules should be marked as external (rollup terminology).
 *
 * On Windows, the builtin module directory is $NODE_HOME/builtin. For all other platforms, the
 * builtin module directory is $NODE_HOME/lib/builtin. This loader file is always in the builtin directory,
 * so the builtin directory is discovered relative to this file.
 *
 * In the case of react, apps should be using the builtin version or react. However, there are legitimate use cases
 * where a developer may want to use a different version of react. So, the loader will prioritize a user specified
 * react over the builtin. The react-reconciler is baked into light-source-react. react-reconciler and react versions
 * are highly coupled (because react-reconciler is a pseudo-internal library of react). So, if the user chooses a
 * react version (or uses a feature) that is incompatible with the baked in react-reconciler, the user will get
 * undefined behavior.
 *
 * @ignore
 */

const builtinPath = dirname(fileURLToPath(import.meta.url))

if (basename(builtinPath) !== 'builtin') {
  throw Error('Fatal: builtin module loader is not in the builtin directory')
}

const userOverridableModules = {
  react: pathToFileURL(resolvePath(builtinPath, 'react', 'index.cjs'))
}
const lightSourceModules = {
  'light-source': pathToFileURL(join(builtinPath, 'light-source', 'index.mjs')),
  'light-source-react': pathToFileURL(join(builtinPath, 'light-source-react', 'index.mjs'))
}

export const resolve = async (specifier, context, defaultResolver) => {
  let replacement

  if ((replacement = lightSourceModules[specifier])) {
    return defaultResolver(replacement, context)
  }

  try {
    return defaultResolver(specifier, context)
  } catch (e) {
    if ((replacement = userOverridableModules[specifier])) {
      return defaultResolver(replacement, context)
    } else {
      throw e
    }
  }
}
