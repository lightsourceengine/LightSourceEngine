/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { load } from '../../src/addon/load.js'
import { bindTestSuite } from 'napi-unit/mocha'

// Run native napi-unit tests.
//
// If the native code was built with the environment variable npm_config_with_native_tests="true", native modules
// will export a 'test' property containing a TestSuite. The TestSuite is run using mocha's test runner and reporter,
// along with the other javascript tests.
//
// If the environment variable is not "true", no tests will be run nor will any tests be reported during the test run.

const bindTestSuiteOptions = {
  // Note: bindings function must be called from this file in order for require to find light-source native modules.
  bindings: (...args) => load(...args),
  testSuiteProperty: 'test'
}

bindTestSuite([
  'light-source',
  'napi-ext-lib-test',
  'light-source-util-lib-test',
  'light-source-platform-lib-test'
], bindTestSuiteOptions)
