/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { terser } from 'rollup-plugin-terser'
import babel from 'rollup-plugin-babel'
import { readFileSync } from 'fs'
import { join } from 'path'

const preamble = '// Light Source Engine\n' +
  '// Copyright (C) 2019 Daniel Anderson.\n' +
  '// This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.'

/**
 * Clean up rollup javascript output with terser.
 */
export const beautify = () => terser({
  compress: false,
  mangle: false,
  output: {
    ecma: 8,
    quote_style: 1,
    semicolons: false,
    beautify: true,
    preamble
  }
})

/**
 * rollup onwarn handler that throws an Error, stopping the rollup process. Most warnings in rollup end up being issues
 * that need to be addressed and should fail the build.
 */
export const onwarn = (warning, warn) => {
  warn(warning)
  throw Error(warning.message)
}

/**
 * Configures the babel rollup plugin with ES6 module transformations disabled.
 *
 * Note, the .babelrc file is configured for runtime targets, such as test or running apps, where babel needs to
 * transform ES6 modules imports to requires. During the rollup process, module transformations are handled by
 * the commonjs plugin. So, preset-env must have the modules settings set to false during rollup.
 *
 * @param babelConfigPath Path to directory containing a .babelrc file.
 */
export const babelPreserveImports = ({ babelConfigPath }) => {
  const rc = JSON.parse(readFileSync(join(babelConfigPath, '.babelrc')))

  if (!rc.presets) {
    throw Error('No presets section in .babelrc file.')
  }

  const env = rc.presets.find(p => Array.isArray(p) && p[0] === '@babel/preset-env')

  if (!env) {
    throw Error('env preset not found in .babelrc file.')
  }

  env[1].modules = false

  return babel({
    ...rc,
    babelrc: false,
    exclude: ['node_modules/**']
  })
}

/**
 * Override module resolution path for require/import statements.
 */
export const overrideResolve = (options = {}) => ({
  resolveId (importee) {
    return options[importee]
  }
})

/**
 * Get the list of exports for each module id. This method gets exports by requiring the
 * module id.
 *
 * @param moduleIds List of module names.
 */
export const getNamedExports = (moduleIds) => {
  const exports = {}

  for (const id of moduleIds) {
    exports[id] = Object.keys(require(id)).filter(x => x !== 'default' && !x.startsWith('__'))
  }

  return exports
}

/**
 * Minify javascript with terser.
 */
export const minify = () => terser({
  mangle: {
    module: true,
  },
  // Disable compress, as the defaults (inlining in particular) cause performance problems in v8.
  compress: false,
  output: {
    ecma: 8,
    quote_style: 1,
    semicolons: false,
    preamble
  }
})
