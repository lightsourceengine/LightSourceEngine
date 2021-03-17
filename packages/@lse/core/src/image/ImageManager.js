/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { CImageManager } from '../addon/index.js'

/**
 * @class module:@lse/core.ImageManager
 */
export class ImageManager {
  _native = new CImageManager()

  // TODO: add preload api

  /**
   * @ignore
   */
  get $native () {
    return this._native
  }
}
