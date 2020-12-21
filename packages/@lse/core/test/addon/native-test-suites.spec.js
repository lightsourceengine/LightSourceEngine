/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { resolve, dirname } from 'path'
import { fileURLToPath } from 'url'
import bindings from 'bindings'
import { bindTestSuite } from 'napi-unit/mocha'

/**
 * The native tests modules need to be loaded from packages/light-source (module_root). By default, bindings will use
 * the invoking file as reference to get the module_root. Since bindTestSuite() is invoking binding, the module_root
 * would resolve to packages/napi-unit. To load the native tests from the correct path, module_root is explicitly
 * set.
 *
 * Note: don't use process.cwd(), as this is dependent on how the tests were invoked.
 *
 * @ignore
 */
const module_root = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..')

const bindTestSuiteOptions = {
  bindings: (addon) => bindings({ bindings: addon + '.node', module_root }),
  testSuiteProperty: 'test'
}

/**
 * Run native napi-unit tests.
 *
 * If the native code was built with the environment variable npm_config_enable_native_tests=1, native modules
 * will export a 'test' property containing a TestSuite. The TestSuite is run using mocha's test runner and reporter,
 * along with the other javascript tests.
 *
 * If the environment variable is 0 or not set, no tests will be run nor will any tests be reported during the test run.
 * 
 * @ignore
 */
bindTestSuite([
  'light-source',
  'lse-lib-napi-ext-test',
  'lse-lib-util-test',
  'lse-lib-platform-test'
], bindTestSuiteOptions)
