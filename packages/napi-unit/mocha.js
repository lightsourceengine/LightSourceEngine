/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

const { describe, it, before, after, beforeEach, afterEach } = require('mocha')

const isTestSuite = (obj) => {
  const { constructor } = Object.getPrototypeOf(obj)

  return constructor && constructor.name === 'TestSuite'
}

const bind = (testSuite) => {
  describe(testSuite.description, () => {
    const func = f => f || (() => {})
    const array = arr => arr || []

    array(testSuite.children).forEach(bindTestSuite);
    before(func(testSuite.before))
    after(func(testSuite.after))
    beforeEach(func(testSuite.beforeEach))
    afterEach(func(testSuite.afterEach))
    array(testSuite.tests).forEach(({description, func}) => it(description, func))
  })
}

/**
 * Converts a napi-unit TestSuite object into a runnable mocha test suite.
 */
const bindTestSuite = (obj, options = {}) => {
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

exports.bindTestSuite = bindTestSuite
