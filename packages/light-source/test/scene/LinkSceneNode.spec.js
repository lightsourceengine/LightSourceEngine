/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { afterSceneTest, beforeSceneTest } from '../test-env'
import { assert } from 'chai'

const kImage = 'test/resources/640x480.png'
const kSvg = 'test/resources/300x300.svg'
const kFont = 'test/resources/arrow.ttf'

describe('LinkSceneNode', () => {
  let scene
  beforeEach(() => { scene = beforeSceneTest() })
  afterEach(() => { scene = afterSceneTest() })
  describe('fetch()', () => {
    it('should get raster image file', async () => {
      const node = scene.createNode('link')

      node.as = 'image'

      const summary = await linkOnLoadAsync(node, kImage)

      assert.equal(node.href, kImage)
      assert.equal(summary.width, 640)
      assert.equal(summary.height, 480)
    })
    it('should get svg image file', async () => {
      const node = scene.createNode('link')

      node.as = 'image'

      const summary = await linkOnLoadAsync(node, kSvg)

      assert.equal(node.href, kSvg)
      assert.equal(summary.width, 300)
      assert.equal(summary.height, 300)
    })
    it('should get svg image file scaled to 500x500', async () => {
      const svg = `file:${kSvg}?width=500&height=500`
      const node = scene.createNode('link')

      node.as = 'image'

      const summary = await linkOnLoadAsync(node, svg)

      assert.equal(node.href, svg)
      assert.equal(summary.width, 500)
      assert.equal(summary.height, 500)
    })
    it('should get font', async () => {
      const node = scene.createNode('link')

      node.as = 'font'

      const summary = await linkOnLoadAsync(node, kFont)

      assert.equal(node.href, kFont)
      assert.equal(summary.family, 'arrow')
      assert.equal(summary.style, 'normal')
      assert.equal(summary.weight, 'normal')
    })
    it('should get font with custom family, style and weight (as = font)', async () => {
      const font = `file:${kFont}?family=ARROW&style=oblique&weight=bold`
      const node = scene.createNode('link')

      node.as = 'font'

      const summary = await linkOnLoadAsync(node, font)

      assert.equal(node.href, font)
      assert.equal(summary.family, 'ARROW')
      assert.equal(summary.style, 'oblique')
      assert.equal(summary.weight, 'bold')
    })
    it('should get font with custom family, style and weight (as = auto)', async () => {
      const font = `file:${kFont}?family=ARROW&style=oblique&weight=bold`
      const node = scene.createNode('link')
      const summary = await linkOnLoadAsync(node, font)

      assert.equal(node.href, font)
      assert.equal(summary.family, 'ARROW')
      assert.equal(summary.style, 'oblique')
      assert.equal(summary.weight, 'bold')
    })
    it('should fallback to filename for missing family param', async () => {
      for (const familyValue of ['&', '']) {
        const font = `file:${kFont}?family=` + familyValue
        const node = scene.createNode('link')
        const summary = await linkOnLoadAsync(node, font)

        assert.equal(node.href, font)
        assert.equal(summary.family, 'arrow')
      }
    })
    it('should fallback to normal for invalid style param', async () => {
      for (const styleValue of ['&', '', 'garbage', 123]) {
        const font = `file:${kFont}?style=` + styleValue
        const node = scene.createNode('link')
        const summary = await linkOnLoadAsync(node, font)

        assert.equal(node.href, font)
        assert.equal(summary.style, 'normal')
      }
    })
    it('should fallback to normal for invalid weight param', async () => {
      for (const weightValue of ['&', '', 'garbage', 123]) {
        const font = `file:${kFont}?weight=` + weightValue
        const node = scene.createNode('link')
        const summary = await linkOnLoadAsync(node, font)

        assert.equal(node.href, font)
        assert.equal(summary.weight, 'normal')
      }
    })
    it('should call onError for invalid filename', async () => {
      const node = scene.createNode('link')

      for (const as of ['auto', 'image', 'font']) {
        node.as = as
        await linkOnErrorAsync(node, 'invalid')
        assert.equal(node.href, 'invalid')
      }
    })
  })
  describe('appendChild()', () => {
    it('should always throw Error', () => {
      assert.throws(() => scene.createNode('link').appendChild(scene.createNode('box')))
    })
  })
})

const linkOnLoadAsync = async (node, href) => {
  return new Promise((resolve, reject) => {
    node.href = href
    node.onLoad = (...args) => resolve(args[1])
    node.onError = (...args) => reject(Error(args[1]))
    node.fetch()
  })
}

const linkOnErrorAsync = async (node, href) => {
  return new Promise((resolve, reject) => {
    node.href = href
    node.onLoad = () => reject(Error('Unexpected onError'))
    node.onError = (...args) => resolve(args[1])
    node.fetch()
  })
}
