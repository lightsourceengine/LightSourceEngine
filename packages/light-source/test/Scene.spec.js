/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { Stage } from '../src/Stage'
import { BoxSceneNode, TextSceneNode, ImageSceneNode } from '../src/addon-light-source'

const $attach = Symbol.for('attach')
const $detach = Symbol.for('detach')

let stage
let scene

describe('Scene', () => {
  beforeEach(() => {
    stage = new Stage()
    stage.init({ adapter: 'light-source-ref' })
    scene = stage.createScene({ width: 1280, height: 720 })
  })
  afterEach(() => {
    stage[Symbol.for('destroy')]()
    stage = null
    scene = null
  })
  describe('createNode()', () => {
    it('should create a new SceneNode by tag name', () => {
      const inputs = [
        ['div', BoxSceneNode],
        ['box', BoxSceneNode],
        ['text', TextSceneNode],
        ['img', ImageSceneNode]
      ]

      for (const input of inputs) {
        assert.instanceOf(scene.createNode(input[0]), input[1])
      }
    })
    it('should throw Error for unsupported tag name', () => {
      for (const tag of ['', 'body', null, undefined]) {
        assert.throws(() => scene.createNode(tag))
      }
    })
  })
  describe('width', () => {
    it('should get renderable width', () => {
      scene[$attach]()
      assert.equal(scene.width, 1280)
    })
  })
  describe('height', () => {
    it('should get renderable height', () => {
      scene[$attach]()
      assert.equal(scene.height, 720)
    })
  })
  describe('fullscreen', () => {
    it('should get fullscreen state', () => {
      scene[$attach]()
      assert.isTrue(scene.fullscreen)
    })
  })
  describe('displayIndex', () => {
    it('should get displayIndex', () => {
      assert.equal(scene.displayIndex, 0)
    })
  })
  describe('stage', () => {
    it('should get stage', () => {
      assert.strictEqual(scene.stage, stage)
    })
  })
  describe('root', () => {
    it('should get root', () => {
      assert.strictEqual(Object.getPrototypeOf(scene.root).constructor.name, 'RootSceneNode')
    })
  })
  describe('title', () => {
    it('should return root', () => {
      scene.title = 'App Title'
      assert.strictEqual(scene.title, 'App Title')
    })
  })
  describe('resource', () => {
    it('should register a new font', () => {
      scene.resource.addFont({ family: 'arrow', uri: 'test/arrow.ttf' })
      assert.lengthOf(scene.resource.fonts, 1)
      assert.include(scene.resource.fonts[0], { family: 'arrow', style: 'normal', weight: 'normal' })
    })
    it('should throw error when font is already registered', () => {
      scene.resource.addFont({ family: 'arrow', uri: 'test/arrow.ttf' })
      assert.throws(() => scene.resource.addFont({ family: 'arrow', uri: 'test/arrow.ttf' }))
      assert.lengthOf(scene.resource.fonts, 1)
    })
    it('should throw an error when passed an invalid font info Object', () => {
      for (const font of [null, '', {}, { family: '' }, { uri: '' }]) {
        assert.throws(() => scene.resource.addFont(font))
      }
      assert.lengthOf(scene.resource.fonts, 0)
    })
  })
  describe('attach()', () => {
    it('should...', () => {
      scene[$attach]()
    })
  })
  describe('detach()', () => {
    it('should...', () => {
      scene[$attach]()
      scene[$detach]()
    })
    it('should...', () => {
      scene[$detach]()
    })
  })
  // describe('frame()', () => {
  //   it('should...', () => {
  //   })
  // })
})
