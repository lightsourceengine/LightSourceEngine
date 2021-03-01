/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { FontManager } from '../../src/font/FontManager.js'
import { Font } from '../../src/font/Font.js'
import { readFileSync } from 'fs'
import { EventName } from '../../src/event/EventName.js'

const { assert } = chai
const testFontFamily = 'arrow'
const testFontFile = 'test/resources/arrow.ttf'
const testFontFileWildcard = 'test/resources/arrow.*'
const testImageFile = 'test/resources/300x300.svg'

describe('FontManager', () => {
  let manager
  beforeEach(() => {
    manager = new FontManager(false)
  })
  afterEach(() => {
    manager.$destroy()
    manager = null
  })
  describe('add()', () => {
    it('should return create and return new font from file', () => {
      testAddTestFont(testFontFile, 'loading')
    })
    it('should return create and return new font from file with wildcard', () => {
      testAddTestFont(testFontFileWildcard, 'loading')
    })
    it('should return create and return new font from buffer', () => {
      testAddTestFont(readFileSync(testFontFile), 'ready')
    })
    it('should dispatch ready event after add from file', async () => {
      await testStatusReadyEvent(testFontFile)
    })
    it('should dispatch ready event after add from buffer', async () => {
      await testStatusReadyEvent(readFileSync(testFontFile))
    })
    it('should dispatch error event if file does not exist', async () => {
      await testStatusErrorEvent('does-not-exist.ttf')
    })
    it('should dispatch error event if file is not a ttf', async () => {
      await testStatusErrorEvent(testImageFile)
    })
    it('should dispatch error event if buffer is not a ttf', async () => {
      await testStatusErrorEvent(readFileSync(testImageFile))
    })

    const testAddTestFont = (uri, expectedStatus) => {
      const font = manager.add({ uri, family: testFontFamily })

      assert.instanceOf(font, Font)
      assert.equal(font.family, testFontFamily)
      assert.equal(font.style, 'normal')
      assert.equal(font.weight, 'normal')
      assert.equal(font.status, expectedStatus)
      assert.isAbove(font.id, 0)
    }

    const testStatusReadyEvent = async (uri) => {
      const font = manager.add({ uri, family: 'arrow' })
      const event = await new Promise((resolve) => {
        font.on(EventName.onStatus, resolve)
      })

      assert.equal(event.type, EventName.onStatus)
      assert.equal(event.target, font)
      assert.equal(event.target.status, 'ready')
      assert.isUndefined(event.error)
    }

    const testStatusErrorEvent = async (uri) => {
      const font = manager.add({ uri, family: testFontFamily })
      const event = await new Promise((resolve) => {
        font.on(EventName.onStatus, resolve)
      })

      assert.equal(event.type, EventName.onStatus)
      assert.equal(event.target.status, 'error')
      assert.isString(event.error)
    }
  })
  describe('property: fonts', () => {
    it('should be empty after construction', () => {
      assert.isEmpty(manager.fonts)
    })
    it('should return added font', () => {
      const font = manager.add({ uri: testFontFile, family: testFontFamily })

      assert.lengthOf(manager.fonts, 1)
      assert.strictEqual(manager.fonts[0], font)
    })
    it('should return all added fonts', () => {
      manager.add({ uri: testFontFile, family: testFontFamily })
      manager.add({ uri: testFontFile, family: testFontFamily, style: 'italic' })
      manager.add({ uri: testFontFile, family: testFontFamily, weight: 'bold' })
      manager.add({ uri: testFontFile, family: testFontFamily, style: 'oblique', weight: 'bold' })
      assert.lengthOf(manager.fonts, 4)
    })
  })
  describe('property: defaultFontFamily', () => {
    it('should be empty after construction', () => {
      assert.isEmpty(manager.defaultFontFamily)
    })
    it('should set default font name', () => {
      manager.defaultFontFamily = 'test'
      assert.equal(manager.defaultFontFamily, 'test')
    })
    it('should use first added font as defaultFontFamily', () => {
      manager.add({ uri: testFontFile, family: testFontFamily })
      assert.equal(manager.defaultFontFamily, testFontFamily)
    })
  })
})
