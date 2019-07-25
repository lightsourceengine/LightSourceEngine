/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "Macros.h"
#include <YGEnums.h>

namespace ls {

SS_ENUM_SEQ_DECL(
    StyleFontWeight,
    StyleFontWeightNormal,
    StyleFontWeightBold
)

SS_ENUM_SEQ_DECL(
    StyleFontStyle,
    StyleFontStyleNormal,
    StyleFontStyleItalic
)

SS_ENUM_SEQ_DECL(
    StyleTextOverflow,
    StyleTextOverflowNone,
    StyleTextOverflowClip,
    StyleTextOverflowEllipsis
)

SS_ENUM_SEQ_DECL(
    StyleTextAlign,
    StyleTextAlignLeft,
    StyleTextAlignCenter,
    StyleTextAlignRight
)

SS_ENUM_SEQ_DECL(
    StyleObjectFit,
    StyleObjectFitFill,
    StyleObjectFitContain,
    StyleObjectFitCover,
    StyleObjectFitNone,
    StyleObjectFitScaleDown
)

SS_ENUM_SEQ_DECL(
    StyleBackgroundClip,
    StyleBackgroundClipBorderBox,
    StyleBackgroundClipPaddingBox
)

SS_ENUM_SEQ_DECL(
    StyleTextTransform,
    StyleTextTransformUppercase,
    StyleTextTransformLowercase
)

SS_ENUM_SEQ_DECL(
    StyleAnchor,
    StyleAnchorTop,
    StyleAnchorRight,
    StyleAnchorBottom,
    StyleAnchorLeft
)

SS_ENUM_SEQ_DECL(
    StyleNumberUnit,
    StyleNumberUnitPoint,
    StyleNumberUnitPercent,
    StyleNumberUnitViewportWidth,
    StyleNumberUnitViewportHeight,
    StyleNumberUnitViewportMin,
    StyleNumberUnitViewportMax,
    StyleNumberUnitAuto,
    StyleNumberUnitAnchor
)

SS_ENUM_SEQ_DECL(
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

template <>
constexpr int32_t Count<YGPositionType>() {
    return facebook::yoga::enums::count<YGPositionType>();
}

template <>
constexpr int32_t Count<YGDisplay>() {
    return facebook::yoga::enums::count<YGDisplay>();
}

template <>
constexpr int32_t Count<YGFlexDirection>() {
    return facebook::yoga::enums::count<YGFlexDirection>();
}

template <>
constexpr int32_t Count<YGJustify>() {
    return facebook::yoga::enums::count<YGJustify>();
}

template <>
constexpr int32_t Count<YGOverflow>() {
    return facebook::yoga::enums::count<YGOverflow>();
}

template <>
constexpr int32_t Count<YGAlign>() {
    return facebook::yoga::enums::count<YGAlign>();
}

template <>
constexpr int32_t Count<YGWrap>() {
    return facebook::yoga::enums::count<YGWrap>();
}

} // namespace ls
