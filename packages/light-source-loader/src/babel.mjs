/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import babel from '@babel/core'
import { getFormatJsx } from './index.mjs'

const { transformAsync } = babel

/**
 * Transpiles jsx, commonjs and es module code with babel using the user's babel configuration.
 *
 * @ignore
 */
export const transformSource = async (source, context, defaultTransformSource) => {
  if (context.format === 'commonjs' || context.format === 'module') {
    const { code } = await transformAsync(source)

    source = code
  }

  return defaultTransformSource(source, context, defaultTransformSource)
}

export { getFormatJsx as getFormat }
