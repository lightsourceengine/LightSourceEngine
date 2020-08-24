/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { fileuri } from '../../src/util'

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
