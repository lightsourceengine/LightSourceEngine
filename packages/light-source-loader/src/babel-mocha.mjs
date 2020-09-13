/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { isJsx, babelTransformSource } from './index.mjs'
import { fileURLToPath } from 'url'
import { dirname, join } from 'path'

/**
 * Module loader for the mocha test environment that transpiles tests using babel.
 *
 * The loader should be invoked in the following way:
 *
 *    node --loader light-source-loader/babel-mocha _mocha <mocha-args>
 *
 * Note: _mocha is an identifier the babel-mocha loader understands.
 *
 * babel can be configured through standard babel configuration files.
 *
 * The loader allows ".jsx" and ".mjsx" files to be imported within a test; however, the mocha commandline does
 * not handle test files with ".jsx" and ".mjsx" extensions. Tests are limited to ".js", ".cjs" and ".mjs". So,
 * the loader's JSX support is limited.
 *
 * Why?
 *
 * mocha has partial support for node's ES modules. mocha correctly imports or requires tests based on file extension
 * and the package.json "type" field. However, the ability to transpile the imported test files is not available. For
 * commonjs, this was done using "mocha -r @babel/register <specs>". -r and @babel/register. For node ES modules,
 * -r no longer works (due to use of require). Further, the mocha commandline program does not have the ability
 * to set an ES module loader (nor the ability to set custom node arguments).
 *
 * Fortunately, mocha can be invoked with a js file in the mocha package (mocha/bin/_mocha):
 * node <package-path>/mocha/bin/_mocha <mocha-args>. With this commandline, the --loader argument can be given to
 * node, allowing mocha to run with a custom ES modules loader for babel transpiling. Also, the babel-mocha will
 * find the _mocha file in the mocha package and correctly interpret it as commonjs.
 *
 * @ignore
 */

export const getFormat = async (url, context, defaultGetFormat) => {
  if (isJsx(url)) {
    return { format: 'module' }
  } else if (url.endsWith('_mocha')) {
    return { format: 'commonjs' }
  } else {
    return defaultGetFormat(url, context, defaultGetFormat)
  }
}

export const resolve = async (specifier, context, defaultResolver) => {
  if (specifier.endsWith('_mocha')) {
    // Find the mocha package. Expecting a url like: file://.../node_modules/mocha/index.js
    const result = await defaultResolver('mocha', context)

    // Find _mocha relative to index.js. getFormat will mark this file as commonjs.
    return defaultResolver(join(dirname(fileURLToPath(result.url)), 'bin', '_mocha'), context)
  }

  return defaultResolver(specifier, context)
}

export { babelTransformSource as transformSource }
