/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */
import { resolve } from './common.mjs'

/**
 * Default Light Source Engine module loader.
 *
 * - Loads pseudo-builtin (global) modules from LSE_PATH.
 * - Adds ability to run mocha bin with a module loader.
 *
 * @ignore
 */

export const getFormat = async (url, context, defaultGetFormat) => url.endsWith('_mocha') ? { format: 'commonjs' } : defaultGetFormat(url, context, defaultGetFormat)

export { resolve }
