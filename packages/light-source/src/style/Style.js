/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { StyleBase, StyleEnumMappings } from '../addon'
import { parseColorValue } from './parseColorValue'
import { isNumber, symbolFor } from '../util'

export class Style extends StyleBase {
  constructor (props) {
    super()

    for (const prop in props) {
      if (prop in StyleBase.prototype) {
        this[prop] = props[prop]
      }
    }

    this[$updateInternalFlags]()
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

  set backgroundWidth (value) { super.backgroundWidth = parsePointValue(value, POINT_PERCENT_REGEX, autoResultFromString, 0) }

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

  set fontSize (value) { super.fontSize = parsePointValue(value, POINT_REGEX, null, 1) }

  get fontStyle () { return StyleEnumMappings.fontStyleToString.get(super.fontStyle) }

  set fontStyle (value) { super.fontStyle = StyleEnumMappings.fontStyleFromString.get(value) }

  get fontWeight () { return StyleEnumMappings.fontWeightToString.get(super.fontWeight) }

  set fontWeight (value) { super.fontWeight = StyleEnumMappings.fontWeightFromString.get(value) }

  get lineHeight () { return super.lineHeight }

  set lineHeight (value) { super.lineHeight = parseLineHeightValue(value) }

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

  get textOverflow () { return StyleEnumMappings.textOverflowToString.get(super.textOverflow) }

  set textOverflow (value) { super.textOverflow = StyleEnumMappings.textOverflowFromString.get(value) }

  get textTransform () { return StyleEnumMappings.textTransformToString.get(super.textTransform) }

  set textTransform (value) { super.textTransform = StyleEnumMappings.textTransformFromString.get(value) }

  get tintColor () { return super.tintColor }

  set tintColor (value) { super.tintColor = parseColorValue(value) }
}

const {
  UnitPoint,
  UnitPercent,
  UnitViewportWidth,
  UnitViewportHeight,
  UnitViewportMin,
  UnitViewportMax,
  UnitRootEm,
  UnitAuto,
  UnitAnchor
} = StyleBase
const $updateInternalFlags = symbolFor('updateInternalFlags')
const parseResult = [0, 0]
const POINT_REGEX = /^(-?\d+\.?\d*)(px|vw|vh|vmin|vmax|rem)$/
const POINT_PERCENT_REGEX = /^(-?\d+\.?\d*)(px|%|vw|vh|vmin|vmax|rem)$/
const autoResultFromString = new Map([
  ['auto', [UnitAuto, 0]]
])
const anchorResultFromString = new Map()

if (StyleEnumMappings.anchorFromString) {
  StyleEnumMappings.anchorFromString.forEach(
    (value, key) => anchorResultFromString.set(key, [UnitAnchor, value]))
}

const unitFromString = new Map([
  ['px', UnitPoint],
  ['%', UnitPercent],
  ['vw', UnitViewportWidth],
  ['vh', UnitViewportHeight],
  ['vmin', UnitViewportMin],
  ['vmax', UnitViewportMax],
  ['rem', UnitRootEm]
])

const parsePointValue = (value, regex, lookup, floor) => {
  let u
  let v

  if (typeof value === 'string') {
    if (lookup && lookup.has(value)) {
      return lookup.get(value)
    } else {
      const match = regex.exec(value)

      if (!match || !unitFromString.has(match[2])) {
        return
      }

      u = unitFromString.get(match[2])
      v = parseFloat(match[1])
    }
  } else if (isNumber(value)) {
    u = UnitPoint
    v = value
  } else {
    return
  }

  if (floor !== -1 && v < floor) {
    return
  }

  parseResult[0] = u
  parseResult[1] = v

  return parseResult
}

const parseLineHeightValue = (value) => {
  let u
  let v

  if (typeof value === 'string') {
    const match = POINT_PERCENT_REGEX.exec(value)

    if (!match || !unitFromString.has(match[2])) {
      return
    }

    u = unitFromString.get(match[2])
    v = parseFloat(match[1])
  } else if (isNumber(value)) {
    u = UnitPercent
    v = value * 100
  } else {
    return
  }

  if (v < 0) {
    return
  }

  parseResult[0] = u
  parseResult[1] = v

  return parseResult
}

const parsePositiveInteger = value => {
  if (isNumber(value) && value >= 0) {
    value = value >>> 0
    parseResult[0] = UnitPoint
    parseResult[1] = value

    return parseResult
  }
}

const parseOpacity = value => {
  if (isNumber(value) && value >= 0 && value <= 1) {
    parseResult[0] = UnitPoint
    parseResult[1] = value

    return parseResult
  }
}
