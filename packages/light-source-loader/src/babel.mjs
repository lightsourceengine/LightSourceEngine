/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { getFormatJsx, babelTransformSource } from './index.mjs'

/**
 * Module loader that transpiles commonjs, module and JSX files using babel.
 *
 * babel can be configured through standard babel configuration files.
 *
 * @ignore
 */

export { getFormatJsx as getFormat, babelTransformSource as transformSource }
