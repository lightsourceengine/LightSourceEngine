/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

const { join } = require('path')

// Use stringify to return the string wrapped in " quotes. Required to get windows filenames working with gyp
exports.include = JSON.stringify(join(__dirname, 'include'))
