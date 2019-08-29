/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import bindings from 'bindings'

const lib = bindings('light-source')

const emptyFunction = () => {}

const renderTestGroup = (group) => {
  describe(group.description, () => {
    before(group.before || emptyFunction)
    after(group.after || emptyFunction)
    beforeEach(group.beforeEach || emptyFunction)
    afterEach(group.afterEach || emptyFunction)

    for (const child of group.children || []) {
      renderTestGroup(child)
    }

    for (const test of group.tests || []) {
      it(test.description, test.func)
    }
  })
}

// If the module was compiled with LIGHT_SOURCE_ENABLE_NATIVE_TESTS defined, the module exports "test". test is a
// TestGroup instance that contains a tree of test information that can be mapped to Mocha's describe and it functions.
// If tests were not compiled, test will be undefined and this test suite will be a no-op.

lib.test && renderTestGroup(lib.test)
