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

import { terser } from 'rollup-plugin-terser'
import replace from 'rollup-plugin-re'
import babel from '@rollup/plugin-babel'
import { readFileSync } from 'fs'

const preamble =
  '// Light Source Engine Version ${version}\n' +
  '// Copyright (c) 2021 Light Source Software, LLC. All rights reserved.\n' +
  '//\n' +
  '// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with\n' +
  '// the License. You may obtain a copy of the License at\n' +
  '//\n' +
  '//     http://www.apache.org/licenses/LICENSE-2.0\n' +
  '//\n' +
  '// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on\n' +
  '// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the\n' +
  '// specific language governing permissions and limitations under the License.\n'

/**
 * Clean up rollup javascript output with terser.
 */
export const beautify = () => terser({
  compress: false,
  mangle: false,
  ecma: 2020,
  output: {
    quote_style: 1,
    semicolons: false,
    beautify: true,
    preamble: preamble.replace("${version}", getPublishingVersion())
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
 * Run babel using the .babelrc file.
 */
export const babelrc = (options = {}) => babel({
  babelHelpers: 'bundled',
  exclude: ['node_modules/**'],
  ...options
})

/**
 * Plugin to replace a module with a javascript code string.
 *
 * @param options Map of module ID to javascript code string.
 */
export const inlineModule = (options = {}) => ({
  name: 'inlineModule',
  resolveId (source) {
    return (source in options) ? source : null
  },
  load (id) {
    return (id in options) ? options[id] : null
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
export const minify = (options = {}) => terser({
  mangle: {
    module: true,
    reserved: options.reserved,
    // property name mangling does not produce working code!
    properties: false
  },
  // Disable compress, as the defaults (inlining in particular) cause performance problems in v8.
  compress: false,
  output: {
    quote_style: 1,
    semicolons: false,
    preamble
  },
  ecma: 2020,
  module: true
})

/**
 * Replaces process.env.NODE_ENV with 'production' string.
 */
export const nodeEnv = () => replace({
  sourceMap: false,
  replaces: {
    'process.env.NODE_ENV': JSON.stringify('production')
  }
})

/**
 * Plugin that does nothing. Used as a placeholder for creating config templates.
 */
export const noop = () => replace()

/**
 * Replaces 'object-assign' imports with the standard Object.assign function.
 */
export const replaceObjectAssign = () => replace({
  sourceMap: false,
  replaces: {
    'require("object-assign")': 'Object.assign',
    'require(\'object-assign\')': 'Object.assign'
  }
})

/**
 * Read the version field from a package.json file.
 */
export const getPackageJsonVersion = (packageJsonFile) => JSON.parse(readFileSync(packageJsonFile, "utf8")).version

let lightSourceVersion

/**
 * Get the monorepo application version.
 */
export const getPublishingVersion = () => {
  if (!lightSourceVersion) {
    let versionFile

    // TODO: version file search patch while refactoring @lse scope.
    try {
      versionFile = readFileSync('../../../publishing/version.json', "utf8")
    } catch (e) {
      versionFile = readFileSync('../../publishing/version.json', "utf8")
    }

    lightSourceVersion = JSON.parse(versionFile).version
  }
  return lightSourceVersion
}
