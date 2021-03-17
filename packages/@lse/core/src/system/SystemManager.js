/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { emptyArray } from '../util/index.js'

/**
 * @memberof module:@lse/core
 * @hideconstructor
 */
class SystemManager {
  _plugin = null
  _displays = emptyArray
  _videoDrivers = emptyArray

  get displays () {
    return this._displays
  }

  get videoDrivers () {
    return this._videoDrivers
  }

  /**
   * @ignore
   */
  $setPlugin (plugin) {
    this._displays = plugin.getDisplays()
    this._videoDrivers = plugin.getVideoDriverNames()
    this._plugin = plugin
  }

  /**
   * @ignore
   */
  $attach () {
  }

  /**
   * @ignore
   */
  $detach () {
  }

  /**
   * @ignore
   */
  $destroy () {
    this._displays = emptyArray
    this._plugin = null
  }

  /**
   * @ignore
   */
  $processEvents () {
    return this._plugin.processEvents()
  }

  /**
   * @ignore
   */
  $createGraphicsContext ({ displayId = 'auto', width = 'auto', height = 'auto', fullscreen = 'auto' }) {
    if (!this._plugin) {
      throw Error('SystemManager has no plugin installed!')
    }

    if (displayId === 'auto') {
      displayId = 0
    } else if (!Number.isInteger(displayId) || displayId < 0 || displayId >= this.displays.length) {
      throw Error(`displayId [${displayId}] out of displays range`)
    }

    if (width === 'auto') {
      width = 0
    } else if (!Number.isInteger(width) || width < 0) {
      throw Error(`width [${width}] must be an integer >= 0`)
    }

    if (height === 'auto') {
      height = 0
    } else if (!Number.isInteger(height) || height < 0) {
      throw Error(`height [${height}] must be an integer >= 0`)
    }

    if (fullscreen === 'auto') {
      fullscreen = true
    } else {
      fullscreen = !!fullscreen
    }

    const display = this.displays[displayId]
    const { defaultMode } = display

    if (fullscreen) {
      if (!width && !height) {
        width = defaultMode.width
        height = defaultMode.height
      } else if (!display.modes.find(mode => mode.width === width && mode.height === height)) {
        [width, height] = findClosestFullscreenSize(display, width, height)
      }
    } else {
      if (!width) {
        width = ~~Math.min(defaultMode.width * 0.80, 1920)
      }

      if (!height) {
        height = ~~Math.min(defaultMode.height * 0.80, 1080)
      }
    }

    return this._plugin.createGraphicsContext({ displayId, width, height, fullscreen })
  }
}

const findClosestFullscreenSize = (display, width, height) => {
  let match

  for (const candidate of display.modes) {
    if (candidate.width && candidate.width < width) {
      break
    }

    if (candidate.height && candidate.height < height) {
      if (candidate.width && candidate.width === width) {
        break
      }
      continue
    }

    if (!match || candidate.width < match.width || candidate.height < match.height) {
      match = candidate
    }
  }

  const closest = {}

  if (match) {
    if (match.width && match.height) {
      closest.width = match.width
      closest.height = match.height
    } else {
      closest.width = width
      closest.height = height
    }
  }

  return [closest.width || 640, closest.height || 480]
}

export { SystemManager }
