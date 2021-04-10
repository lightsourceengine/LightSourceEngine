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
import { join } from 'path'
import { readFileSync } from 'fs'
import { afterSceneTest, beforeSceneTest } from '../test-env.js'
import { ImageSceneNode } from '../../src/scene/SceneNode.js'
import { atob } from '../../src/util/index.js'

const { assert } = chai
const imagePath = 'test/resources'
const images = ['640x480.png', '300x300.svg', '600x400.jpg', '600x400.gif']
const rasterImages = ['640x480.png', '600x400.jpg', '600x400.gif']
const svgXmlWithNewLines = `<svg width="300" height="300" xmlns="http://www.w3.org/2000/svg">
  <rect x="2" y="2" width="296" height="296" style="fill:#DEDEDE;stroke:#555555;stroke-width:2"/>
</svg>`
const svgXml = svgXmlWithNewLines.replace(/(\r\n|\n|\r)/gm, '')

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
  beforeEach(() => { scene = beforeSceneTest() })
  afterEach(() => { scene = afterSceneTest() })
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
        assert.equal(node.src.uri, input)
      }
    })
    it('should be assignable to a relative file path', async () => {
      return testImageUri(images.map(value => join(imagePath, value)))
    })
    it('should be assignable to a raster image data uri', async () => {
      return testImageUri(rasterImages.map(value => {
        return `data:image/image;base64,${readFileSync(join(imagePath, value)).toString('base64')}`
      }))
    })
    it('should be assignable to a data uri with svg xml', async () => {
      return testImageUri(`data:image/svg+xml;utf8,${svgXml}`)
    })
    it('should be assignable to a data uri with svg xml (with new lines)', async () => {
      return testImageUri(`data:image/svg+xml;utf8,${svgXmlWithNewLines}`)
    })
    it('should be assignable to a data uri with base64 encoded svg xml', async () => {
      return testImageUri(`data:image/svg+xml;base64,${atob(svgXml)}`)
    })
    it('should be assignable to a data uri with base64 encoded svg xml (with new lines)', async () => {
      return testImageUri(`data:image/svg+xml;base64,${atob(svgXmlWithNewLines)}`)
    })
    it('should be assignable to an image path without extension', async () => {
      return testImageUri('test/resources/640x480.*')
    })
    it('should be assignable to an invalid image path, but return error', async () => {
      scene.stage.start()

      const node = scene.createNode('img')
      const uri = 'test/resources/invalid.jpg'

      node.src = uri
      assert.equal(node.src.uri, uri)

      await expectOnError(node)
    })
    it('should be null when assigned a non-string value', () => {
      const node = scene.createNode('img')

      for (const input of ['', null, undefined, 3, {}, { id: 'image.jpg' }]) {
        node.src = input
        assert.isEmpty(node.src.uri)
      }
    })
  })
  const testImageUri = async (uris) => {
    uris = Array.isArray(uris) ? uris : [uris]
    scene.stage.start()
    const promises = []

    for (const uri of uris) {
      const node = scene.createNode('img')

      node.src = uri
      assert.equal(node.src.uri, uri)

      promises.push(expectOnLoad(node))
    }

    await Promise.all(promises)
  }
})
