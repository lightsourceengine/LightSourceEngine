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
import { fileuri } from '../../src/util/index.js'

const { assert } = chai
const invalidPathValues = ['', 123, {}, [], null, undefined, NaN]

describe('fileuri()', () => {
  it('should build file uri from path and no parameters', () => {
    assert.equal(fileuri('path/to/file.txt'), 'file:path/to/file.txt')
  })
  it('should build file uri from path and one integer parameter', () => {
    assert.equal(fileuri('path/to/file.txt', { w: 100 }), 'file:path/to/file.txt?w=100')
  })
  it('should build file uri from path and one string parameter', () => {
    assert.equal(fileuri('path/to/file.txt', { str: 'value' }), 'file:path/to/file.txt?str=value')
  })
  it('should build file uri from path and one encode-able string parameter', () => {
    assert.equal(fileuri('path/to/file.txt', { str: 'value+' }), 'file:path/to/file.txt?str=value%2B')
  })
  it('should build file uri from path and two integer parameters', () => {
    assert.equal(fileuri('path/to/file.txt', { w: 100, h: 100 }), 'file:path/to/file.txt?w=100&h=100')
  })
  it('should build file uri from path with + character', () => {
    assert.equal(fileuri('name+with+plus.txt'), 'file:name%2Bwith%2Bplus.txt')
  })
  it('should build file uri from path with % character', () => {
    assert.equal(fileuri('name%with%percent.txt'), 'file:name%25with%25percent.txt')
  })
  it('should return empty string for invalid path arg', () => {
    for (const path of invalidPathValues) {
      assert.equal(fileuri(path), '')
    }
  })
  it('should return empty string for invalid path arg and valid parameter', () => {
    for (const path of invalidPathValues) {
      assert.equal(fileuri(path, { w: 100 }), '')
    }
  })
})
