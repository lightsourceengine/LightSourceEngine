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
    StyleBackgroundRepeat,
    StyleBackgroundRepeatXY,
    StyleBackgroundRepeatX,
    StyleBackgroundRepeatY,
    StyleBackgroundRepeatOff
)

LS_ENUM_SEQ_DECL(
    StyleBackgroundSize,
    StyleBackgroundSizeNone,
    StyleBackgroundSizeCover,
    StyleBackgroundSizeContain
)

LS_ENUM_SEQ_DECL(
    StyleNumberUnit,
    StyleNumberUnitUndefined,
    StyleNumberUnitPoint,
    StyleNumberUnitPercent,
    StyleNumberUnitViewportWidth,
    StyleNumberUnitViewportHeight,
    StyleNumberUnitViewportMin,
    StyleNumberUnitViewportMax,
    StyleNumberUnitAuto,
    StyleNumberUnitAnchor,
    StyleNumberUnitRootEm,
    StyleNumberUnitRadian,
    StyleNumberUnitDegree,
    StyleNumberUnitGradian,
    StyleNumberUnitTurn
)

LS_ENUM_SEQ_DECL(
    StyleTransform,
    StyleTransformIdentity,
    StyleTransformTranslate,
    StyleTransformRotate,
    StyleTransformScale
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
    backgroundHeight,
    backgroundImage,
    backgroundPositionX,
    backgroundPositionY,
    backgroundRepeat,
    backgroundSize,
    backgroundWidth,
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
    tintColor,
    transform,
    transformOriginX,
    transformOriginY
)

constexpr bool IsYogaLayoutProperty(StyleProperty property) noexcept {
    return property <= StyleProperty::width;
}

// Add ToString, FromString and Count to Yoga enum types for a consistent enum interface across style properties.

#define LS_GEN_ENUM_FUNCTIONS(ENUM)                          \
    ENUM ENUM##FromString(const char* value);                \
    template<>                                               \
    inline ENUM FromString(const char* value) {              \
        return ENUM##FromString(value);                      \
    }                                                        \
    template <>                                              \
    inline const char* ToString<ENUM>(ENUM value) noexcept { \
        return ENUM##ToString(value);                        \
    }                                                        \
    template <>                                              \
    constexpr int32_t Count<ENUM>() noexcept {               \
        return facebook::yoga::enums::count<ENUM>();         \
    }

LS_GEN_ENUM_FUNCTIONS(YGAlign)
LS_GEN_ENUM_FUNCTIONS(YGDisplay)
LS_GEN_ENUM_FUNCTIONS(YGFlexDirection)
LS_GEN_ENUM_FUNCTIONS(YGJustify)
LS_GEN_ENUM_FUNCTIONS(YGOverflow)
LS_GEN_ENUM_FUNCTIONS(YGPositionType)
LS_GEN_ENUM_FUNCTIONS(YGWrap)

#undef LS_GEN_ENUM_FUNCTIONS

} // namespace ls
