/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import React from 'react'
import { container, beforeEachTestCase, afterEachTestCase, rejects, renderAsync } from './test-env'
import { render } from '../src'

const kImage720 = 'test/resource/image-1280x720.png'
const kImage720SearchExt = 'test/resource/image-1280x720.*'
const kImage1080 = 'test/resource/image-1920x1080.png'
const kSoundFile = 'test/resource/move.wav'
let root

describe('ImageElement', () => {
  beforeEach(() => { beforeEachTestCase(); root = container() })
  afterEach(async () => { root = null; await afterEachTestCase() })
  describe('prop: src', () => {
    it('should call onLoad when image loaded', async () => {
      const summary = await renderImage(kImage720)

      assert.equal(root.children[0].src, kImage720)
      assert.deepStrictEqual(summary, { width: 1280, height: 720 })
    })
    it('should call onLoad when image loaded by search extension', async () => {
      const summary = await renderImage(kImage720SearchExt)

      assert.equal(root.children[0].src, kImage720SearchExt)
      assert.deepStrictEqual(summary, { width: 1280, height: 720 })
    })
    it('should call onError when file loaded by search extension fails', async () => {
      const file = 'unknown.*'

      await rejects(renderImage(file))

      assert.equal(root.children[0].src, file)
    })
    it('should call onError when file not found', async () => {
      const file = 'unknown.jpg'

      await rejects(renderImage(file))

      assert.equal(root.children[0].src, file)
    })
    it('should call onError when file exists, but file is not an image', async () => {
      const file = kSoundFile

      await rejects(renderImage(file))

      assert.equal(root.children[0].src, file)
    })
    it('should not call callbacks when src not specified', async () => {
      await renderImageNoSrc()

      assert.equal(root.children[0].src, '')
    })
    it('should call onLoad when replacing an image', async () => {
      // 1. Set <img> src to an image.
      let summary = await renderImage(kImage720)

      const img = root.children[0]

      assert.equal(img.src, kImage720)
      assert.deepStrictEqual(summary, { width: 1280, height: 720 })

      // 2. Replace <img> src with a different image.
      const setSrc = new Promise((resolve, reject) => {
        img.onLoad = (...args) => resolve(args[1])
        img.onError = (...args) => reject(Error(args[1]))
        img.src = kImage1080
      })

      summary = await setSrc

      assert.equal(img.src, kImage1080)
      assert.deepStrictEqual(summary, { width: 1920, height: 1080 })
    })
    it('should set src to empty string for invalid values', async () => {
      for (const invalidSrc of [1234, {}, true, [], null, undefined, '']) {
        await renderAsync(<img src={invalidSrc} />)
        assert.isEmpty(root.children[0].src)
      }
    })
  })
})

const renderImage = async (src) => {
  return await new Promise((resolve, reject) => {
    render(
      root,
      <img
        src={src}
        onLoad={(...args) => resolve(args[1])}
        onError={(...args) => reject(Error(args[1]))}
      />
    )
  })
}

const renderImageNoSrc = async () => {
  return await new Promise((resolve, reject) => {
    render(
      root,
      <img
        onLoad={(...args) => reject(Error(args[1]))}
        onError={(...args) => reject(Error(args[1]))}
      />,
      resolve)
  })
}
