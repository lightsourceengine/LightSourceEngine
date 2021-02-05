/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { createFontManagerComposite, FontStatus, FontStyle, FontWeight, logger } from '../addon/index.js'
import { promises } from 'fs'
import { Font } from './Font.js'
import { createErrorStatusEvent, createReadyStatusEvent } from '../event/index.js'

const { readFile } = promises

/**
 * Font Manager.
 */
export class FontManager {
  _fonts = new Map()
  _composite = createFontManagerComposite()
  _defaultFontFamily = ''

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
    this._composite?.setDefaultFontFamily(value)
    this._defaultFontFamily = value
  }

  /**
   * Add a font to the font manager.
   *
   * @param spec Font options
   * @returns {Font}
   */
  add (spec) {
    const { _composite, _fonts } = this
    const { src, family, style = 'normal', weight = 'normal', index = 0 } = spec

    if (!Buffer.isBuffer(src) && !(typeof src === 'string')) {
      throw Error(`src - expected: buffer or string, got: ${src}`)
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

    const id = _composite.createFont(family, fontStyleToInt.get(style), fontWeightToInt.get(weight))

    if (!id) {
      throw Error(`'${family}':${style}:${weight} - create failed`)
    }

    const font = new Font(id, family, style, weight, statusToString(_composite.getStatus(id)))

    _fonts.set(id, font)

    if (_fonts.size === 1 && !this.defaultFontFamily) {
      // Set the first font as the default font family.
      this.defaultFontFamily = family
    }

    if (typeof src === 'string') {
      font.$status = statusToString(_composite.getStatus(id))

      logger.info(`'${family}':${style}:${weight} - ${src}`, 'font')

      readFile(src)
        .then((buffer) => {
          setBufferAndDispatch(this, id, buffer, index, false)
        })
        .catch((e) => {
          logger.error(`'${family}':${style}:${weight}: ${e.message}`, 'font')

          const font = _fonts.get(id)

          if (font) {
            _composite.setError(id)
            font.$status = statusToString(FontStatus.ERROR)
            font.dispatchEvent(createErrorStatusEvent(font, e.message))
          }
        })
    } else {
      setBufferAndDispatch(this, id, src, index, true)
    }

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
    if (!this._composite) {
      return
    }

    this._fonts.clear()
    this._composite.destroy()
    this._composite = null
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

const setBufferAndDispatch = (self, id, buffer, index, defer) => {
  const { _composite, _fonts } = self

  if (!_composite || !_fonts.has(id)) {
    // it's possible the font manager could have been destroyed while fetching font file or the font
    // was removed. if so, bail.
    return
  }

  const font = _fonts.get(id)

  // setBuffer will move the font resource to a terminal state, either ready or error
  _composite.setBuffer(id, buffer)

  const status = _composite.getStatus(id)

  font.$status = statusToString(status)

  logger.info(`'${font.family}':${font.style}:${font.weight}: ${font.$status}`, 'font')

  let event

  if (status === FontStatus.READY) {
    // Note:
    // The native layer needs this buffer. I prefer to hold it here, rather than trying to manage
    // references at the native layer.
    font.$buffer = buffer
    event = createReadyStatusEvent(font)
  } else {
    event = createErrorStatusEvent(font, `'${font.family}':${font.style}:${font.weight} - not a valid font src`)
  }

  if (defer) {
    queueMicrotask(() => font.dispatchEvent(event))
  } else {
    font.dispatchEvent(event)
  }
}
