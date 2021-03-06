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
import { SystemManager } from '../../src/system/SystemManager.mjs'
import sinon from 'sinon'

const { assert } = chai

describe('SystemManager', () => {
  let system
  let plugin
  beforeEach(() => {
    plugin = {
      getDisplays () {
        return [{
          id: 0,
          defaultMode: {
            width: 1920,
            height: 1080
          },
          modes: [
            {
              width: 1920,
              height: 1080
            }
          ]
        }]
      },
      getVideoDriverNames () {
        return ['test']
      },
      createGraphicsContext (options) {
        return {}
      },
      processEvents: sinon.stub()
    }
    plugin.createGraphicsContextSpy = sinon.spy(plugin, 'createGraphicsContext')
    system = new SystemManager()
    system.$setPlugin(plugin)
  })
  afterEach(() => {
    system = null
    plugin = null
  })
  describe('property: displays', () => {
    it('should return displays from plugin', () => {
      assert.lengthOf(system.displays, 1)
    })
  })
  describe('property: videoDrivers', () => {
    it('should return videoDrivers from plugin', () => {
      assert.lengthOf(system.videoDrivers, 1)
    })
  })
  describe('$processEvents()', () => {
    it('should call processEvents() on plugin', () => {
      system.$processEvents()
      assert.isTrue(plugin.processEvents.called)
    })
  })
  describe('$createGraphicsContext()', () => {
    it('should set width and height for windowed context', () => {
      system.$createGraphicsContext({ width: 123, height: 456, fullscreen: false })

      expectCreateGraphicsContextToBeCalledWith({ displayId: 0, width: 123, height: 456, fullscreen: false })
    })
    it('should use default values when option property is auto or undefined', () => {
      const defaults = [
        {},
        { width: 0, height: 0, fullscreen: true },
        { displayId: 0, width: 0, height: 0 },
        { width: 0, height: 0, fullscreen: true },
        { width: 'auto', height: 'auto', fullscreen: 'auto' }
      ]

      for (const options of defaults) {
        system.$createGraphicsContext(options)

        expectCreateGraphicsContextToBeCalledWith({ displayId: 0, width: 1920, height: 1080, fullscreen: true })
        plugin.createGraphicsContextSpy.resetHistory()
      }
    })
    it('should use default width and height when fullscreen = false', () => {
      const defaults = [
        { width: 0, fullscreen: false },
        { width: 'auto', fullscreen: false },
        { height: 0, fullscreen: false },
        { height: 'auto', fullscreen: false },
        { width: 0, height: 0, fullscreen: false },
        { width: 'auto', height: 'auto', fullscreen: false }
      ]

      for (const options of defaults) {
        system.$createGraphicsContext(options)

        expectCreateGraphicsContextToBeCalledWith({ displayId: 0, width: ~~(1920 * 0.8), height: ~~(1080 * 0.8), fullscreen: false })
        plugin.createGraphicsContextSpy.resetHistory()
      }
    })
    it('should throw error for invalid displayId', () => {
      for (const displayId of [-1, 999, null, '', [], {}, NaN]) {
        assert.throws(() => system.$createGraphicsContext({ displayId }))
      }
    })
    it('should throw error for invalid width', () => {
      for (const width of [-1, null, '', [], {}, NaN]) {
        assert.throws(() => system.$createGraphicsContext({ width }))
      }
    })
    it('should throw error for invalid height', () => {
      for (const height of [-1, null, '', [], {}, NaN]) {
        assert.throws(() => system.$createGraphicsContext({ height }))
      }
    })
  })
  describe('$destroy()', () => {
    it('should clear displays after destroy', () => {
      system.$destroy()
      assert.lengthOf(system.displays, 0)
    })
  })

  const expectCreateGraphicsContextToBeCalledWith = ({ displayId, width, height, fullscreen }) => {
    const { createGraphicsContextSpy } = plugin
    const args = createGraphicsContextSpy.firstCall.args[0]

    assert.isTrue(createGraphicsContextSpy.called)
    assert.equal(args.displayId, displayId)
    assert.equal(args.width, width)
    assert.equal(args.height, height)
    assert.equal(args.fullscreen, fullscreen)
  }
})
