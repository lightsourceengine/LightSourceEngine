/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import bindings from 'bindings'
import { buildTestSuite } from 'napi-unit/mocha'

const lib = bindings('light-source')

// If the module was compiled with LIGHT_SOURCE_ENABLE_NATIVE_TESTS defined, the module exports "test". test is a
// TestSuite instance that contains a tree of test information that can be mapped to Mocha's describe and it functions.
// If tests were not compiled, test will be undefined and this test suite will be a no-op.

lib.test && buildTestSuite(lib.test)
