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

import { CFontManager, FontStatus, FontStyle, FontWeight, logger } from '../addon/index.mjs'
import { Font } from './Font.mjs'
import { createErrorStatusEvent, createReadyStatusEvent } from '../event/index.mjs'
import { join, isAbsolute, normalize } from 'path'
import { readFileSync } from 'fs'
import { isDataUri } from '../util/index.mjs'
import { getBuiltinFont } from './getBuiltinFont.cjs'

const kDataUriFontRegEx = /data:font\/[+\-\w\d]+;base64,(.*)/

/**
 * Font Manager.
 *
 * @memberof module:@lse/core
 * @hideconstructor
 */
class FontManager {
  _fonts = new Map()
  _native = new CFontManager()
  _defaultFontFamily = ''

  constructor (enableBootstrapFonts = true) {
    enableBootstrapFonts && bootstrapFonts(this)
  }

  /**
   * List of fonts currently available in the font manager. The fonts in this array are no particular order.
   */
  get fonts () {
    return Array.from(this._fonts.values())
  }

  /**
   * The font family name Style uses when fontFamily is not set.
   *
   * @returns {string}
   */
  get defaultFontFamily () {
    return this._defaultFontFamily
  }

  set defaultFontFamily (value) {
    if (typeof value !== 'string') {
      throw Error('defaultFontFamily - expected: string')
    }
    this._native?.setDefaultFontFamily(value)
    this._defaultFontFamily = value
  }

  /**
   * Add a font to the font manager.
   *
   * @param spec Font options
   * @returns {Font}
   */
  add (spec) {
    const { _native, _fonts } = this
    const { uri, family, style = 'normal', weight = 'normal', index = 0 } = spec

    if (!Buffer.isBuffer(uri) && !(typeof uri === 'string')) {
      throw Error(`uri - expected: buffer or string, got: ${uri}`)
    }

    if (typeof family !== 'string' || !family) {
      throw Error(`family - expected: string, got: ${family}`)
    }

    if (!fontStyleToInt.has(style)) {
      throw Error(`style - expected: ${Array.from(fontStyleToInt.values()).join(', ')} got: ${style}`)
    }

    if (!fontWeightToInt.has(weight)) {
      throw Error(`weight - expected: ${Array.from(fontWeightToInt.values()).join(', ')} got: ${weight}`)
    }

    if (!Number.isInteger(index)) {
      throw Error(`index - expected: integer, got: ${index}`)
    }

    let exists = false

    for (const [, value] of _fonts) {
      if (value.family === family && value.style === style && value.weight === weight) {
        exists = true
        break
      }
    }

    if (exists) {
      throw Error(`'${family}':${style}:${weight} - already exists`)
    }

    const id = _native.createFont(family, fontStyleToInt.get(style), fontWeightToInt.get(weight))

    if (!id) {
      throw Error(`'${family}':${style}:${weight} - create failed`)
    }

    const font = new Font(id, family, style, weight, statusToString(_native.getStatus(id)))

    _fonts.set(id, font)

    if (_fonts.size === 1 && !this.defaultFontFamily) {
      // Set the first font as the default font family.
      this.defaultFontFamily = family
    }

    let status

    if (typeof uri === 'string') {
      if (isDataUri(uri)) {
        const result = uri.match(kDataUriFontRegEx)

        // TODO: decoding could be put on a native background thread
        if (result) {
          status = _native.loadFontFromBuffer(id, Buffer.from(result[1], 'base64'), index)
        } else {
          status = _native.loadFontFromBuffer(id, null, index)
        }
      } else {
        const callback = (id, fontStatus) => {
          processStatusChange(this, id, fontStatus, false)
        }

        status = _native.loadFontFromFile(id, uri, index, callback)
      }
    } else {
      status = _native.loadFontFromBuffer(id, uri, index)
    }

    processStatusChange(this, id, status, true)

    return font
  }

  /**
   * @ignore
   */
  $attach () {
    // nothing to attach
  }

  /**
   * @ignore
   */
  $detach () {
    // nothing to detach, fonts stay live during a suspend
  }

  /**
   * @ignore
   */
  $destroy () {
    if (!this._native) {
      return
    }

    this._fonts.clear()
    this._native.destroy()
    this._native = null
  }

  /**
   * @ignore
   */
  get $native () {
    return this._native
  }
}

const fontStyleToInt = new Map([
  ['normal', FontStyle.NORMAL],
  ['italic', FontStyle.ITALIC],
  ['oblique', FontStyle.OBLIQUE]
])

const fontWeightToInt = new Map([
  ['normal', FontWeight.NORMAL],
  ['bold', FontWeight.BOLD]
])

const statusToString = (statusEnum) => {
  switch (statusEnum) {
    case FontStatus.LOADING:
      return 'loading'
    case FontStatus.READY:
      return 'ready'
    case FontStatus.ERROR:
      return 'error'
    default:
      return 'unknown'
  }
}

const processStatusChange = (self, id, newStatus, defer) => {
  const font = self._fonts.get(id)

  if (!font) {
    return
  }

  newStatus = newStatus ?? FontStatus.ERROR
  font.$status = statusToString(newStatus)
  logger.info(`'${font.family}':${font.style}:${font.weight} - ${font.status}`, 'font')

  let event

  switch (newStatus) {
    case FontStatus.READY:
      event = createReadyStatusEvent(font)
      break
    case FontStatus.ERROR:
      event = createErrorStatusEvent(font, 'Failed to load font.')
      break
    default:
      // wait for a terminal state font status
      break
  }

  if (event) {
    if (defer) {
      queueMicrotask(() => font.dispatchEvent(event))
    } else {
      font.dispatchEvent(event)
    }
  }
}

const bootstrapFonts = (self) => {
  const { LSE_FONT_PATH, LSE_ENV } = process.env
  let fontManifest

  if (LSE_FONT_PATH) {
    // If LSE_FONT_PATH is set, load the font manifest in this directory.
    const fontManifestFile = join(LSE_FONT_PATH, 'font.manifest')

    try {
      fontManifest = JSON.parse(readFileSync(fontManifestFile, 'utf8'))
    } catch (e) {
      logger.warn(`Error loading '${fontManifestFile}': ${e.message}`)
      return
    }

    if (!Array.isArray(fontManifest)) {
      logger.warn(`Expected array from '${fontManifestFile}'`)
      return
    }

    for (const entry of fontManifest) {
      if (typeof entry.uri === 'string' && !isAbsolute(entry.uri)) {
        entry.uri = normalize(join(LSE_FONT_PATH, entry.uri))
      }
    }
  } else if (LSE_ENV !== 'lse-node') {
    // If LSE_FONT_PATH is NOT set and the environment is NOT lse-node, load the specific
    // "builtin" ttf. This is to cover the use case of running from the mono repo or an
    // project that installs @lse/core from npm.
    fontManifest = [
      {
        uri: getBuiltinFont(),
        family: 'roboto-builtin'
      }
    ]
  }

  for (const spec of fontManifest) {
    try {
      self.add(spec)
    } catch (e) {
      logger.warn(`font.manifest contains invalid spec: ${spec.uri}`)
    }
  }
}

export { FontManager }
