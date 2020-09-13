/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

export const isJsx = (url) => url.endsWith('.jsx') || url.endsWith('.mjsx')

export const getFormatJsx = async (url, context, defaultGetFormat) =>
  isJsx(url) ? { format: 'module' } : defaultGetFormat(url, context, defaultGetFormat)
