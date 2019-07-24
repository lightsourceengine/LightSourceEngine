/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { StyleBase, StyleEnumMappings } from './addon-light-source'

export const ColorNames = {
  // from: https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#Color_keywords
  aliceblue: 0xF0F8FF,
  antiquewhite: 0xFAEBD7,
  aqua: 0x00FFFF,
  aquamarine: 0x7FFFD4,
  azure: 0xF0FFFF,
  beige: 0xF5F5DC,
  bisque: 0xFFE4C4,
  black: 0x000000,
  blanchedalmond: 0xFFEBCD,
  blue: 0x0000FF,
  blueviolet: 0x8A2BE2,
  brown: 0xA52A2A,
  burlywood: 0xDEB887,
  cadetblue: 0x5F9EA0,
  chartreuse: 0x7FFF00,
  chocolate: 0xD2691E,
  coral: 0xFF7F50,
  cornflowerblue: 0x6495ED,
  cornsilk: 0xFFF8DC,
  crimson: 0xDC143C,
  cyan: 0x00FFFF,
  darkblue: 0x00008B,
  darkcyan: 0x008B8B,
  darkgoldenrod: 0xB8860B,
  darkgray: 0xA9A9A9,
  darkgreen: 0x006400,
  darkgrey: 0xA9A9A9,
  darkkhaki: 0xBDB76B,
  darkmagenta: 0x8B008B,
  darkolivegreen: 0x556B2F,
  darkorange: 0xFF8C00,
  darkorchid: 0x9932CC,
  darkred: 0x8B0000,
  darksalmon: 0xE9967A,
  darkseagreen: 0x8FBC8F,
  darkslateblue: 0x483D8B,
  darkslategray: 0x2F4F4F,
  darkslategrey: 0x2F4F4F,
  darkturquoise: 0x00CED1,
  darkviolet: 0x9400D3,
  deeppink: 0xFF1493,
  deepskyblue: 0x00BFFF,
  dimgray: 0x696969,
  dimgrey: 0x696969,
  dodgerblue: 0x1E90FF,
  firebrick: 0xB22222,
  floralwhite: 0xFFFAF0,
  forestgreen: 0x228B22,
  fuchsia: 0xFF00FF,
  gainsboro: 0xDCDCDC,
  ghostwhite: 0xF8F8FF,
  gold: 0xFFD700,
  goldenrod: 0xDAA520,
  gray: 0x808080,
  green: 0x008000,
  greenyellow: 0xADFF2F,
  grey: 0x808080,
  honeydew: 0xF0FFF0,
  hotpink: 0xFF69B4,
  indianred: 0xCD5C5C,
  indigo: 0x4B0082,
  ivory: 0xFFFFF0,
  khaki: 0xF0E68C,
  lavender: 0xE6E6FA,
  lavenderblush: 0xFFF0F5,
  lawngreen: 0x7CFC00,
  lemonchiffon: 0xFFFACD,
  lightblue: 0xADD8E6,
  lightcoral: 0xF08080,
  lightcyan: 0xE0FFFF,
  lightgoldenrodyellow: 0xFAFAD2,
  lightgray: 0xD3D3D3,
  lightgreen: 0x90EE90,
  lightgrey: 0xD3D3D3,
  lightpink: 0xFFB6C1,
  lightsalmon: 0xFFA07A,
  lightseagreen: 0x20B2AA,
  lightskyblue: 0x87CEFA,
  lightslategray: 0x778899,
  lightslategrey: 0x778899,
  lightsteelblue: 0xB0C4DE,
  lightyellow: 0xFFFFE0,
  lime: 0x00FF00,
  limegreen: 0x32CD32,
  linen: 0xFAF0E6,
  magenta: 0xFF00FF,
  maroon: 0x800000,
  mediumaquamarine: 0x66CDAA,
  mediumblue: 0x0000CD,
  mediumorchid: 0xBA55D3,
  mediumpurple: 0x9370DB,
  mediumseagreen: 0x3CB371,
  mediumslateblue: 0x7B68EE,
  mediumspringgreen: 0x00FA9A,
  mediumturquoise: 0x48D1CC,
  mediumvioletred: 0xC71585,
  midnightblue: 0x191970,
  mintcream: 0xF5FFFA,
  mistyrose: 0xFFE4E1,
  moccasin: 0xFFE4B5,
  navajowhite: 0xFFDEAD,
  navy: 0x000080,
  oldlace: 0xFDF5E6,
  olive: 0x808000,
  olivedrab: 0x6B8E23,
  orange: 0xFFA500,
  orangered: 0xFF4500,
  orchid: 0xDA70D6,
  palegoldenrod: 0xEEE8AA,
  palegreen: 0x98FB98,
  paleturquoise: 0xAFEEEE,
  palevioletred: 0xDB7093,
  papayawhip: 0xFFEFD5,
  peachpuff: 0xFFDAB9,
  peru: 0xCD853F,
  pink: 0xFFC0CB,
  plum: 0xDDA0DD,
  powderblue: 0xB0E0E6,
  purple: 0x800080,
  red: 0xFF0000,
  rosybrown: 0xBC8F8F,
  royalblue: 0x4169E1,
  saddlebrown: 0x8B4513,
  salmon: 0xFA8072,
  sandybrown: 0xF4A460,
  seagreen: 0x2E8B57,
  seashell: 0xFFF5EE,
  sienna: 0xA0522D,
  silver: 0xC0C0C0,
  skyblue: 0x87CEEB,
  slateblue: 0x6A5ACD,
  slategray: 0x708090,
  slategrey: 0x708090,
  snow: 0xFFFAFA,
  springgreen: 0x00FF7F,
  steelblue: 0x4682B4,
  tan: 0xD2B48C,
  teal: 0x008080,
  thistle: 0xD8BFD8,
  tomato: 0xFF6347,
  turquoise: 0x40E0D0,
  violet: 0xEE82EE,
  wheat: 0xF5DEB3,
  white: 0xFFFFFF,
  whitesmoke: 0xF5F5F5,
  yellow: 0xFFFF00,
  yellowgreen: 0x9ACD32,
  rebeccapurple: 0x663399
}

const autoResultFromString = new Map([
  ['auto', [StyleBase.UnitAuto, 0]]
])

const anchorResultFromString = new Map()

StyleEnumMappings.anchorFromString.forEach((value, key) => anchorResultFromString.set(key, [StyleBase.UnitAnchor, value]))

const parseResult = [0, 0]

const ALPHABIT = 4294967296 // = 0xFFFFFFFF + 1 = 0x100000000
const POINT_REGEX = /^(-?\d+\.?\d*)(px|vw|vh|vmin|vmax)$/
const POINT_PERCENT_REGEX = /^(-?\d+\.?\d*)(px|%|vw|vh|vmin|vmax)$/

const unitFromString = new Map([
  ['px', StyleBase.UnitPoint],
  ['%', StyleBase.UnitPercent],
  ['vw', StyleBase.UnitViewportWidth],
  ['vh', StyleBase.UnitViewportHeight],
  ['vmin', StyleBase.UnitViewportMin],
  ['vmax', StyleBase.UnitViewportMax]
])

const hexCharCodeToInt = new Map()

for (let i = 0; i < 16; i++) {
  const hexDigit = i.toString(16)

  hexCharCodeToInt.set(hexDigit.toLowerCase().charCodeAt(0), i)
  hexCharCodeToInt.set(hexDigit.toUpperCase().charCodeAt(0), i)
}

const ErrorHexColorString = value => { Error(`Malformed hex color string in Style: ${value}.`) }
const ThrowErrorUnknownColorName = value => { throw Error(`Unsupported color name in Style: ${value}`) }

const fromHexHashString = value => {
  let color = 0
  const len = value.length

  switch (len) {
    case 4:
    case 5:
      for (let i = 1; i < len; i++) {
        const code = value.charCodeAt(i)

        if (!hexCharCodeToInt.has(code)) {
          throw ErrorHexColorString(value)
        }

        const x = hexCharCodeToInt.get(code)

        color = (color << 8) | (x << 4) | x
      }
      break
    case 7:
    case 9:
      for (let i = 1; i < len; i++) {
        const code = value.charCodeAt(i)

        if (!hexCharCodeToInt.has(code)) {
          throw ErrorHexColorString(value)
        }

        color = (color << 4) | hexCharCodeToInt.get(code)
      }
      break
    default:
      throw ErrorHexColorString(value)
  }

  return (len === 5 || len === 9 ? ALPHABIT : 0) + (color >>> 0)
}

const parseColorValue = value => {
  if (typeof value === 'string') {
    if (value.charCodeAt(0) === 35 /* # */) {
      return fromHexHashString(value)
    }

    return ColorNames[value.toLowerCase()] || ThrowErrorUnknownColorName(value)
  }

  return value & 0xFFFFFF
}

const parsePointValue = (value, regex, lookup, floor) => {
  let u
  let v

  if (typeof value === 'string') {
    if (lookup && lookup.has(value)) {
      return lookup.get(value)
    } else {
      const match = regex.exec(value)

      if (!match || !unitFromString.has(match[2])) {
        return undefined
      }

      u = unitFromString.get(match[2])
      v = parseFloat(match[1])
    }
  } else if (typeof value === 'number') {
    u = StyleBase.UnitPoint
    v = value
  } else {
    return undefined
  }

  if (floor !== -1 && v < floor) {
    return undefined
  }

  parseResult[0] = u
  parseResult[1] = v

  return parseResult
}

const parsePositiveInteger = value => {
  if (typeof value === 'number') {
    value = value >>> 0
    if (value >= 0) {
      parseResult[0] = StyleBase.UnitPoint
      parseResult[1] = value

      return parseResult
    }
  }

  return undefined
}

const parseOpacity = value => {
  if (typeof value === 'number' && value >= 0 && value <= 1) {
    parseResult[0] = StyleBase.UnitPoint
    parseResult[1] = value

    return parseResult
  }

  return undefined
}

class Style extends StyleBase {
  constructor (props) {
    super()

    for (const prop in props) {
      if (prop in StyleBase.prototype) {
        this[prop] = props[prop]
      }
    }
  }

  // Layout

  get alignItems () { return StyleEnumMappings.alignToString.get(super.alignItems) }

  set alignItems (value) { super.alignItems = StyleEnumMappings.alignFromString.get(value) }

  get alignContent () { return StyleEnumMappings.alignToString.get(super.alignContent) }

  set alignContent (value) { super.alignContent = StyleEnumMappings.alignFromString.get(value) }

  get alignSelf () { return StyleEnumMappings.alignToString.get(super.alignSelf) }

  set alignSelf (value) { super.alignSelf = StyleEnumMappings.alignFromString.get(value) }

  get border () { return super.border }

  set border (value) { super.border = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get borderBottom () { return super.borderBottom }

  set borderBottom (value) { super.borderBottom = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get borderLeft () { return super.borderLeft }

  set borderLeft (value) { super.borderLeft = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get borderRight () { return super.borderRight }

  set borderRight (value) { super.borderRight = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get borderTop () { return super.borderTop }

  set borderTop (value) { super.borderTop = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get bottom () { return super.bottom }

  set bottom (value) { super.bottom = parsePointValue(value, POINT_PERCENT_REGEX, undefined, -1) }

  get display () { return StyleEnumMappings.displayToString.get(super.display) }

  set display (value) { super.display = StyleEnumMappings.displayFromString.get(value) }

  get flex () { return super.flex }

  set flex (value) { super.flex = parsePositiveInteger(value) }

  get flexBasis () { return super.flexBasis }

  set flexBasis (value) { super.flexBasis = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, 0) }

  get flexDirection () { return super.flexDirection }

  set flexDirection (value) { super.flexDirection = StyleEnumMappings.flexDirectionFromString.get(value) }

  get flexGrow () { return super.flexGrow }

  set flexGrow (value) { super.flexGrow = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, 0) }

  get flexShrink () { return super.flexShrink }

  set flexShrink (value) { super.flexShrink = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, 0) }

  get flexWrap () { return StyleEnumMappings.flexWrapToString.get(super.flexWrap) }

  set flexWrap (value) { super.flexWrap = StyleEnumMappings.flexWrapFromString.get(value) }

  get height () { return super.height }

  set height (value) { super.height = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, -1) }

  get justifyContent () { return StyleEnumMappings.justifyToString.get(super.justifyContent) }

  set justifyContent (value) { super.justifyContent = StyleEnumMappings.justifyFromString.get(value) }

  get left () { return super.left }

  set left (value) { super.left = parsePointValue(value, POINT_PERCENT_REGEX, undefined, -1) }

  get margin () { return super.margin }

  set margin (value) { super.margin = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, -1) }

  get marginBottom () { return super.marginBottom }

  set marginBottom (value) { super.marginBottom = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, -1) }

  get marginLeft () { return super.marginLeft }

  set marginLeft (value) { super.marginLeft = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, -1) }

  get marginRight () { return super.marginRight }

  set marginRight (value) { super.marginRight = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, -1) }

  get marginTop () { return super.marginTop }

  set marginTop (value) { super.marginTop = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, -1) }

  get maxHeight () { return super.maxHeight }

  set maxHeight (value) { super.maxHeight = parsePointValue(value, POINT_PERCENT_REGEX, undefined, 0) }

  get maxWidth () { return super.maxWidth }

  set maxWidth (value) { super.maxWidth = parsePointValue(value, POINT_PERCENT_REGEX, undefined, 0) }

  get minHeight () { return super.minHeight }

  set minHeight (value) { super.minHeight = parsePointValue(value, POINT_PERCENT_REGEX, undefined, 0) }

  get minWidth () { return super.minWidth }

  set minWidth (value) { super.minWidth = parsePointValue(value, POINT_PERCENT_REGEX, undefined, 0) }

  get overflow () { return StyleEnumMappings.overflowToString.get(super.overflow) }

  set overflow (value) { super.overflow = StyleEnumMappings.overflowFromString.get(value) }

  get padding () { return super.padding }

  set padding (value) { super.padding = parsePointValue(value, POINT_PERCENT_REGEX, undefined, 0) }

  get paddingBottom () { return super.paddingBottom }

  set paddingBottom (value) { super.paddingBottom = parsePointValue(value, POINT_PERCENT_REGEX, undefined, 0) }

  get paddingLeft () { return super.paddingLeft }

  set paddingLeft (value) { super.paddingLeft = parsePointValue(value, POINT_PERCENT_REGEX, undefined, 0) }

  get paddingRight () { return super.paddingRight }

  set paddingRight (value) { super.paddingRight = parsePointValue(value, POINT_PERCENT_REGEX, undefined, 0) }

  get paddingTop () { return super.paddingTop }

  set paddingTop (value) { super.paddingTop = parsePointValue(value, POINT_PERCENT_REGEX, undefined, 0) }

  get position () { return super.position }

  set position (value) { super.position = StyleEnumMappings.positionFromString.get(value) }

  get right () { return StyleEnumMappings.positionToString.get(super.right) }

  set right (value) { super.right = parsePointValue(value, POINT_PERCENT_REGEX, undefined, -1) }

  get top () { return super.top }

  set top (value) { super.top = parsePointValue(value, POINT_PERCENT_REGEX, undefined, -1) }

  get width () { return super.width }

  set width (value) { super.width = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, -1) }

  // Visual

  get backgroundClip () { return StyleEnumMappings.backgroundClipToString.get(super.backgroundClip) }

  set backgroundClip (value) { super.backgroundClip = StyleEnumMappings.backgroundClipFromString.get(value) }

  get backgroundColor () { return super.backgroundColor }

  set backgroundColor (value) { super.backgroundColor = parseColorValue(value) }

  get backgroundFit () { return StyleEnumMappings.objectFitToString.get(super.backgroundFit) }

  set backgroundFit (value) { super.backgroundFit = StyleEnumMappings.objectFitFromString.get(value) }

  get backgroundHeight () { return super.backgroundHeight }

  set backgroundHeight (value) { super.backgroundHeight = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, -1) }

  // get backgroundImage () { return super.backgroundImage }
  // set backgroundImage (value) { super.backgroundImage = value }

  get backgroundPositionX () { return super.backgroundPositionX }

  set backgroundPositionX (value) { super.backgroundPositionX = parsePointValue(value, POINT_PERCENT_REGEX, anchorResultFromString, -1) }

  get backgroundPositionY () { return super.backgroundPositionY }

  set backgroundPositionY (value) { super.backgroundPositionY = parsePointValue(value, POINT_PERCENT_REGEX, anchorResultFromString, -1) }

  get backgroundWidth () { return super.backgroundWidth }

  set backgroundWidth (value) { super.backgroundWidth = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, -1) }

  get borderColor () { return super.borderColor }

  set borderColor (value) { super.borderColor = parseColorValue(value) }

  get borderRadius () { return super.borderRadius }

  set borderRadius (value) { super.borderRadius = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get borderRadiusTopLeft () { return super.borderRadiusTopLeft }

  set borderRadiusTopLeft (value) { super.borderRadiusTopLeft = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get borderRadiusTopRight () { return super.borderRadiusTopRight }

  set borderRadiusTopRight (value) { super.borderRadiusTopRight = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get borderRadiusBottomLeft () { return super.borderRadiusBottomLeft }

  set borderRadiusBottomLeft (value) { super.borderRadiusBottomLeft = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get borderRadiusBottomRight () { return super.borderRadiusBottomRight }

  set borderRadiusBottomRight (value) { super.borderRadiusBottomRight = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get color () { return super.color }

  set color (value) { super.color = parseColorValue(value) }

  // get fontFamily () { return super.fontFamily }
  // set fontFamily (value) { super.fontFamily = value }

  // TODO: support %
  get fontSize () { return super.fontSize }

  set fontSize (value) { super.fontSize = parsePointValue(value, POINT_REGEX, undefined, 0) }

  get fontStyle () { return StyleEnumMappings.fontStyleToString.get(super.fontStyle) }

  set fontStyle (value) { super.fontStyle = StyleEnumMappings.fontStyleFromString.get(value) }

  get fontWeight () { return StyleEnumMappings.fontWeightToString.get(super.fontWeight) }

  set fontWeight (value) { super.fontWeight = StyleEnumMappings.fontWeightFromString.get(value) }

  get maxLines () { return super.maxLines }

  set maxLines (value) { super.maxLines = parsePositiveInteger(value) }

  get objectFit () { return StyleEnumMappings.objectFitToString.get(super.objectFit) }

  set objectFit (value) { super.objectFit = StyleEnumMappings.objectFitFromString.get(value) }

  get objectPositionX () { return super.objectPositionX }

  set objectPositionX (value) { super.objectPositionX = parsePointValue(value, POINT_PERCENT_REGEX, anchorResultFromString, -1) }

  get objectPositionY () { return super.objectPositionY }

  set objectPositionY (value) { super.objectPositionY = parsePointValue(value, POINT_PERCENT_REGEX, anchorResultFromString, -1) }

  get opacity () { return super.opacity }

  set opacity (value) { super.opacity = parseOpacity(value) }

  get textAlign () { return StyleEnumMappings.textAlignToString.get(super.textAlign) }

  set textAlign (value) { super.textAlign = StyleEnumMappings.textAlignFromString.get(value) }

  get textOverflow () { return StyleEnumMappings.textOverflowTpString.get(super.textOverflow) }

  set textOverflow (value) { super.textOverflow = StyleEnumMappings.textOverflowFromString.get(value) }

  get textTransform () { return StyleEnumMappings.textTransformToString.get(super.textTransform) }

  set textTransform (value) { super.textTransform = StyleEnumMappings.textTransformFromString.get(value) }

  get tintColor () { return super.tintColor }

  set tintColor (value) { super.tintColor = parseColorValue(value) }

  static rgb (...args) {
    if (args.length === 1) {
      return parseColorValue(args[0])
    } else if (args.length === 3) {
      return (((args[0] & 0xFF) << 16) | ((args[1] & 0xFF) << 8) | (args[2] & 0xFF)) >>> 0
    } else {
      throw Error()
    }
  }

  static rgba (...args) {
    let a
    let color

    if (args.length === 2) {
      a = (args[1] * 255) >>> 0
      color = parseColorValue(args[0])
    } else if (args.length === 4) {
      a = (args[3] * 255) >>> 0
      color = ((args[0] & 0xFF) << 16 | (args[1] & 0xFF) << 8 | (args[2] & 0xFF)) >>> 0
    } else {
      throw Error()
    }

    return ((a & 0xFF) << 24 | color) + ALPHABIT
  }
}

export { Style }
