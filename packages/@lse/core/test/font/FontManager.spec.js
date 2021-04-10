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
import { FontManager } from '../../src/font/FontManager.js'
import { Font } from '../../src/font/Font.js'
import { readFileSync } from 'fs'
import { EventName } from '../../src/event/EventName.js'

const { assert } = chai
const testFontFamily = 'arrow'
const testFontFile = 'test/resources/arrow.ttf'
const testFontFileWildcard = 'test/resources/arrow.*'
const testImageFile = 'test/resources/300x300.svg'
const toFontUri = (buffer, mediaType) => `data:font/${mediaType};base64,${buffer.toString('base64')}`

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
    it('should return create and return new font from data uri', () => {
      testAddTestFont(toFontUri(readFileSync(testFontFile), 'font'), 'ready')
    })
    it('should return font in error state for invalid data uri', () => {
      testAddTestFont('data:invalid', 'error')
    })
    it('should return font in error state for data uri with invalid media type', () => {
      testAddTestFont('data:font/&&&;base64,', 'error')
    })
    it('should return font in error state for data uri with missing base64 data', () => {
      testAddTestFont('data:font/font;base64,', 'error')
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
        font.on(EventName.status, resolve)
      })

      assert.equal(event.type, EventName.status)
      assert.equal(event.target, font)
      assert.equal(event.target.status, 'ready')
      assert.isUndefined(event.error)
    }

    const testStatusErrorEvent = async (uri) => {
      const font = manager.add({ uri, family: testFontFamily })
      const event = await new Promise((resolve) => {
        font.on(EventName.status, resolve)
      })

      assert.equal(event.type, EventName.status)
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
