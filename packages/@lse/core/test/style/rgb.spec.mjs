/*
/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import chai from 'chai'
import { rgb } from '../../src/style/rgb.mjs'

const { assert } = chai

describe('rgb()', () => {
  it('should return integer in 0xRRGGBB color format', () => {
    assert.equal(rgb(0x66, 0x33, 0x99), 0xFF663399)
  })
  it('should clamp out of range color component values', () => {
    assert.equal(rgb(256, 300, 1000), 0xFFFFFFFF)
    assert.equal(rgb(-1, -200, 0xFF), 0xFF0000FF)
  })
  it('should return opaque black when arguments are non-number color components', () => {
    assert.equal(rgb(NaN, NaN, NaN), 0xFF000000)
  })
})
