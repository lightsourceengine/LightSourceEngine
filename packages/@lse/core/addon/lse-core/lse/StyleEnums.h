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

#pragma once

#include <lse/EnumSequence.h>
#include <lse/StyleProperty.h>
#include <YGEnums.h>

namespace lse {

LSE_ENUM_SEQ_DECL(
    StyleFontWeight,
    StyleFontWeightNormal,
    StyleFontWeightBold
)

LSE_ENUM_SEQ_DECL(
    StyleFontStyle,
    StyleFontStyleNormal,
    StyleFontStyleItalic,
    StyleFontStyleOblique
)

LSE_ENUM_SEQ_DECL(
    StyleTextOverflow,
    StyleTextOverflowNone,
    StyleTextOverflowClip,
    StyleTextOverflowEllipsis
)

LSE_ENUM_SEQ_DECL(
    StyleTextAlign,
    StyleTextAlignLeft,
    StyleTextAlignCenter,
    StyleTextAlignRight
)

LSE_ENUM_SEQ_DECL(
    StyleObjectFit,
    StyleObjectFitFill,
    StyleObjectFitContain,
    StyleObjectFitCover,
    StyleObjectFitNone,
    StyleObjectFitScaleDown
)

LSE_ENUM_SEQ_DECL(
    StyleBackgroundClip,
    StyleBackgroundClipBorderBox,
    StyleBackgroundClipPaddingBox,
    StyleBackgroundClipContentBox
)

LSE_ENUM_SEQ_DECL(
    StyleTextTransform,
    StyleTextTransformNone,
    StyleTextTransformUppercase,
    StyleTextTransformLowercase
)

LSE_ENUM_SEQ_DECL(
    StyleAnchor,
    StyleAnchorTop,
    StyleAnchorRight,
    StyleAnchorBottom,
    StyleAnchorLeft,
    StyleAnchorCenter
)

LSE_ENUM_SEQ_DECL(
    StyleBackgroundRepeat,
    StyleBackgroundRepeatOff,
    StyleBackgroundRepeatXY,
    StyleBackgroundRepeatX,
    StyleBackgroundRepeatY
)

LSE_ENUM_SEQ_DECL(
    StyleBackgroundSize,
    StyleBackgroundSizeNone,
    StyleBackgroundSizeCover,
    StyleBackgroundSizeContain
)

LSE_ENUM_SEQ_DECL(
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

LSE_ENUM_SEQ_DECL(
    StyleTransform,
    StyleTransformIdentity,
    StyleTransformTranslate,
    StyleTransformRotate,
    StyleTransformScale
)

LSE_ENUM_SEQ_DECL(
    StyleFilter,
    StyleFilterTint,
    StyleFilterFlipV,
    StyleFilterFlipH
)

// If root does not have a font size set, this value is used for rem calculation.
constexpr const float DEFAULT_REM_FONT_SIZE = 16.f;

// Add ToString, FromString and Count to Yoga enum types for a consistent enum interface across style properties.

#define LSE_GEN_ENUM_FUNCTIONS(ENUM)                         \
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

LSE_GEN_ENUM_FUNCTIONS(YGAlign)
LSE_GEN_ENUM_FUNCTIONS(YGDisplay)
LSE_GEN_ENUM_FUNCTIONS(YGFlexDirection)
LSE_GEN_ENUM_FUNCTIONS(YGJustify)
LSE_GEN_ENUM_FUNCTIONS(YGOverflow)
LSE_GEN_ENUM_FUNCTIONS(YGPositionType)
LSE_GEN_ENUM_FUNCTIONS(YGWrap)

#undef LSE_GEN_ENUM_FUNCTIONS

// Checks if an int32_t can be cast to an enum of type T. The enum must have been declared with LSE_ENUM_SEQ_DECL to
// be compatible with this function.
template<typename T>
constexpr bool IsValidEnum(int32_t value) {
  return value >= 0 && value < Count<T>();
}

} // namespace lse
