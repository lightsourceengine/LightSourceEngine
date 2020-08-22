/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { afterSceneTest, beforeSceneTest } from '../test-env'
import { ImageSceneNode } from '../../src/addon'
import { assert } from 'chai'
import { join } from 'path'

const images = ['640x480.png', '300x300.svg', '600x400.jpg', '600x400.gif']

const expectOnLoad = (node) => new Promise((resolve, reject) => {
  node.onLoad = () => resolve()
  node.onError = () => reject(Error('unexpected onError call'))
})

const expectOnError = (node) => new Promise((resolve, reject) => {
  node.onLoad = () => reject(Error('unexpected onLoad call'))
  node.onError = () => resolve()
})

describe('ImageSceneNode', () => {
  let scene
  before(() => scene = beforeSceneTest())
  after(() => scene = afterSceneTest())
  describe('constructor()', () => {
    it('should create uninitialized node when passed an invalid Scene', () => {
      for (const input of [null, undefined, {}]) {
        assert.throws(() => new ImageSceneNode(input))
      }
    })
  })
  describe('appendChild()', () => {
    it('should always throw Error', () => {
      assert.throws(() => scene.createNode('img').appendChild(scene.createNode('img')))
    })
  })
  describe('removeChild()', () => {
    it('should be a no-op', () => {
      const node = scene.createNode('img')
      node.removeChild(scene.createNode('box'))
      assert.lengthOf(node.children, 0)
    })
  })
  describe('insertBefore()', () => {
    it('should always throw Error', () => {
      assert.throws(() => scene.createNode('img').insertBefore(scene.createNode('img'), scene.createNode('img')))
    })
  })
  describe('src', () => {
    it('should be assignable to a uri string', () => {
      for (const input of images) {
        const node = scene.createNode('img')

        node.src = input
        assert.equal(node.src, input)
      }
    })
    it('should be assignable to a relative file path', async () => {
      scene.stage.start()
      const promises = []

      for (const input of images) {
        const node = scene.createNode('img')
        const uri = join('test/resources', input)

        node.src = uri
        assert.equal(node.src, uri)

        promises.push(expectOnLoad(node))
      }

      await Promise.all(promises)
    })
    it('should be assignable to a resource uri', async () => {
      scene.stage.start()
      const promises = []

      for (const input of images) {
        const node = scene.createNode('img')
        const uri = `file:test/resources/${input}`

        node.src = uri
        assert.equal(node.src, uri)

        promises.push(expectOnLoad(node))
      }

      await Promise.all(promises)
    })
    xit('should be assignable to an svg uri', async () => {
      scene.stage.start()

      const node = scene.createNode('img')
      const uri = 'data:image/svg+xml,<svg width="300" height="300" xmlns="http://www.w3.org/2000/svg">' +
        '<rect x="2" y="2" width="296" height="296" style="fill:#DEDEDE;stroke:#555555;stroke-width:2"/></svg>'

      node.src = uri
      assert.equal(node.src, uri)

      await expectOnLoad(node)
    })
    it('should be assignable to an image path without extension', async () => {
      scene.stage.start()

      const node = scene.createNode('img')
      const uri = 'test/resources/640x480.*'

      node.src = uri
      assert.equal(node.src, uri)

      await expectOnLoad(node)
    })
    it('should be assignable to an invalid image path, but return error', async () => {
      scene.stage.start()

      const node = scene.createNode('img')
      const uri = 'test/resources/invalid.jpg'

      node.src = uri
      assert.equal(node.src, uri)

      await expectOnError(node)
    })
    it('should be null when assigned a non-string value', () => {
      const node = scene.createNode('img')

      for (const input of ['', null, undefined, 3, {}, { id: 'image.jpg' }]) {
        node.src = input
        assert.isEmpty(node.src)
      }
    })
  })
})
