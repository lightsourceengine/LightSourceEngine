/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import React from 'react'
import { root, container, before, after } from './test-env'

const testImagePath = 'test/640x480.png'

describe('ImageElement', () => {
  beforeEach(before)
  afterEach(after)
  describe('prop: src', () => {
    it('should call onLoad after image loaded', async () => {
      await new Promise((resolve, reject) => {
        container()
          .render(<img src={testImagePath}
            onLoad={resolve}
            onError={reject} />)
      })

      assert.isObject(root().children[0].src)
      assert.equal(root().children[0].src.uri, testImagePath)
    })
    it('should call onError when file without extension not found', async () => {
      const file = 'unknown'

      await new Promise((resolve, reject) => container().render(<img src={file} onError={resolve} onLoad={reject} />))

      assert.isObject(root().children[0].src)
      assert.equal(root().children[0].src.uri, file)
    })
    it('should call onError when file not found', async () => {
      const file = 'unknown.jpg'

      await new Promise((resolve, reject) => container().render(<img src={file} onError={resolve} onLoad={reject} />))

      assert.isObject(root().children[0].src)
      assert.equal(root().children[0].src.uri, file)
    })
    it('should not call callbacks when src not specified', async () => {
      await new Promise((resolve, reject) => container().render(<img onError={reject} onLoad={reject} />, resolve))

      assert.isNull(root().children[0].src)
    })
  })
})
