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

import mocha from 'mocha'

const { describe, it, xit, before, after, beforeEach, afterEach } = mocha

const isTestSuite = (obj) => {
  return obj && obj.hasOwnProperty('tests') && obj.hasOwnProperty('description')
}

const bind = (testSuite) => {
  describe(testSuite.description, () => {
    const func = f => f || (() => {})
    const array = arr => arr || []

    array(testSuite.children).forEach(value => bindTestSuite(value));
    before(func(testSuite.before))
    after(func(testSuite.after))
    beforeEach(func(testSuite.beforeEach))
    afterEach(func(testSuite.afterEach))
    array(testSuite.tests).forEach(({description, func, skip}) => (skip ? xit : it)(description, func))
  })
}

/**
 * Converts a napi-unit TestSuite object into a runnable mocha test suite.
 */
export const bindTestSuite = (obj, options = {}) => {
  if (!options.testSuiteProperty) {
    options.testSuiteProperty = 'test'
  }

  let sources

  if (Array.isArray(obj)) {
    sources = obj
  } else {
    sources = [ obj ]
  }

  for (const source of sources) {
    let testSuite

    if (typeof source === 'string') {
      if (typeof options.bindings !== 'function') {
        throw Error('No bindings function passed in options. Cannot load node module: ' + source)
      }

      let module

      try {
        module = options.bindings(source)
      } catch (e) {
        // TODO: add option to throw when module not loaded
        module = {}
      }

      testSuite = module[options.testSuiteProperty]
    } else if (isTestSuite(source)) {
      testSuite = source
    } else {
      throw Error('bindTestSuite expects a TestSuite instance or a string that resolves to a ' +
        'node module that contains a TestSuite. Got: ' + source)
    }

    // TODO: add option to throw when no test suite
    testSuite && bind(testSuite)
  }
}
