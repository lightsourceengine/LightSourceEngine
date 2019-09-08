/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

const { describe, it, before, after, beforeEach, afterEach } = require('mocha')

/**
 * Converts a napi-unit TestSuite object into a runnable mocha test suite.
 */
const buildTestSuite = (testSuite) => {
  describe(testSuite.description, () => {
    const func = f => f || (() => {})
    const array = arr => arr || []

    array(testSuite.children).forEach(buildTestSuite);
    before(func(testSuite.before))
    after(func(testSuite.after))
    beforeEach(func(testSuite.beforeEach))
    afterEach(func(testSuite.afterEach))
    array(testSuite.tests).forEach(({description, func}) => it(description, func))
  })
}

exports.buildTestSuite = buildTestSuite
