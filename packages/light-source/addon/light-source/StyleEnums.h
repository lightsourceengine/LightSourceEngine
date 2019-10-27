/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/EnumSequence.h>
#include <YGEnums.h>

namespace ls {

LS_ENUM_SEQ_DECL(
    StyleFontWeight,
    StyleFontWeightNormal,
    StyleFontWeightBold
)

LS_ENUM_SEQ_DECL(
    StyleFontStyle,
    StyleFontStyleNormal,
    StyleFontStyleItalic
)

LS_ENUM_SEQ_DECL(
    StyleTextOverflow,
    StyleTextOverflowNone,
    StyleTextOverflowClip,
    StyleTextOverflowEllipsis
)

LS_ENUM_SEQ_DECL(
    StyleTextAlign,
    StyleTextAlignLeft,
    StyleTextAlignCenter,
    StyleTextAlignRight
)

LS_ENUM_SEQ_DECL(
    StyleObjectFit,
    StyleObjectFitFill,
    StyleObjectFitContain,
    StyleObjectFitCover,
    StyleObjectFitNone,
    StyleObjectFitScaleDown
)

LS_ENUM_SEQ_DECL(
    StyleBackgroundClip,
    StyleBackgroundClipBorderBox,
    StyleBackgroundClipPaddingBox
)

LS_ENUM_SEQ_DECL(
    StyleTextTransform,
    StyleTextTransformNone,
    StyleTextTransformUppercase,
    StyleTextTransformLowercase
)

LS_ENUM_SEQ_DECL(
    StyleAnchor,
    StyleAnchorTop,
    StyleAnchorRight,
    StyleAnchorBottom,
    StyleAnchorLeft
)

LS_ENUM_SEQ_DECL(
    StyleNumberUnit,
    StyleNumberUnitPoint,
    StyleNumberUnitPercent,
    StyleNumberUnitViewportWidth,
    StyleNumberUnitViewportHeight,
    StyleNumberUnitViewportMin,
    StyleNumberUnitViewportMax,
    StyleNumberUnitAuto,
    StyleNumberUnitAnchor,
    StyleNumberUnitRootEm
)

LS_ENUM_SEQ_DECL(
    StyleProperty,
    // (Yoga) Layout Style Properties
    alignItems,
    alignContent,
    alignSelf,
    border,
    borderBottom,
    borderLeft,
    borderRight,
    borderTop,
    bottom,
    display,
    flex,
    flexBasis,
    flexDirection,
    flexGrow,
    flexShrink,
    flexWrap,
    height,
    justifyContent,
    left,
    margin,
    marginBottom,
    marginLeft,
    marginRight,
    marginTop,
    maxHeight,
    maxWidth,
    minHeight,
    minWidth,
    overflow,
    padding,
    paddingBottom,
    paddingLeft,
    paddingRight,
    paddingTop,
    position,
    right,
    top,
    width,

    // Extended Style Properties

    backgroundClip,
    backgroundColor,
    backgroundImage,
    backgroundFit,
    backgroundPositionX,
    backgroundPositionY,
    borderColor,
    borderRadius,
    borderRadiusTopLeft,
    borderRadiusTopRight,
    borderRadiusBottomLeft,
    borderRadiusBottomRight,
    color,
    fontFamily,
    fontSize,
    fontStyle,
    fontWeight,
    lineHeight,
    maxLines,
    objectFit,
    objectPositionX,
    objectPositionY,
    opacity,
    textAlign,
    textOverflow,
    textTransform,
    tintColor
)

// maps yoga style enums to the Count() function, so all style enums look the same
template <typename E>
constexpr int32_t Count() noexcept {
    return facebook::yoga::enums::count<E>();
}

} // namespace ls
