/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/EnumSequence.h>
#include <ls/StyleProperty.h>
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
    StyleFontStyleItalic,
    StyleFontStyleOblique
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
    StyleBackgroundClipPaddingBox,
    StyleBackgroundClipContentBox
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
    StyleAnchorLeft,
    StyleAnchorCenter
)

LS_ENUM_SEQ_DECL(
    StyleBackgroundRepeat,
    StyleBackgroundRepeatOff,
    StyleBackgroundRepeatXY,
    StyleBackgroundRepeatX,
    StyleBackgroundRepeatY
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

// If root does not have a font size set, this value is used for rem calculation.
constexpr const float DEFAULT_REM_FONT_SIZE = 16.f;

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

// Checks if an int32_t can be cast to an enum of type T. The enum must have been declared with LS_ENUM_SEQ_DECL to
// be compatible with this function.
template<typename T>
constexpr bool IsValidEnum(int32_t value) {
    return value >= 0 && value < Count<T>();
}

} // namespace ls