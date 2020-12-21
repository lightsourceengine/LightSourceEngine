/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <lse/EnumSequence.h>
#include <functional>

namespace lse {

#define LS_FOR_EACH_STYLE_PROPERTY(APPLY) \
     APPLY(alignItems) \
     APPLY(alignContent) \
     APPLY(alignSelf) \
     APPLY(border) \
     APPLY(borderBottom) \
     APPLY(borderLeft) \
     APPLY(borderRight) \
     APPLY(borderTop) \
     APPLY(bottom) \
     APPLY(display) \
     APPLY(flex) \
     APPLY(flexBasis) \
     APPLY(flexDirection) \
     APPLY(flexGrow) \
     APPLY(flexShrink) \
     APPLY(flexWrap) \
     APPLY(height) \
     APPLY(justifyContent) \
     APPLY(left) \
     APPLY(margin) \
     APPLY(marginBottom) \
     APPLY(marginLeft) \
     APPLY(marginRight) \
     APPLY(marginTop) \
     APPLY(maxHeight) \
     APPLY(maxWidth) \
     APPLY(minHeight) \
     APPLY(minWidth) \
     APPLY(overflow) \
     APPLY(padding) \
     APPLY(paddingBottom) \
     APPLY(paddingLeft) \
     APPLY(paddingRight) \
     APPLY(paddingTop) \
     APPLY(position) \
     APPLY(right) \
     APPLY(top) \
     APPLY(width) \
     APPLY(backgroundClip) \
     APPLY(backgroundColor) \
     APPLY(backgroundHeight) \
     APPLY(backgroundImage) \
     APPLY(backgroundPositionX) \
     APPLY(backgroundPositionY) \
     APPLY(backgroundRepeat) \
     APPLY(backgroundSize) \
     APPLY(backgroundWidth) \
     APPLY(borderColor) \
     APPLY(borderRadius) \
     APPLY(borderRadiusTopLeft) \
     APPLY(borderRadiusTopRight) \
     APPLY(borderRadiusBottomLeft) \
     APPLY(borderRadiusBottomRight) \
     APPLY(color) \
     APPLY(fontFamily) \
     APPLY(fontSize) \
     APPLY(fontStyle) \
     APPLY(fontWeight) \
     APPLY(lineHeight) \
     APPLY(maxLines) \
     APPLY(objectFit) \
     APPLY(objectPositionX) \
     APPLY(objectPositionY) \
     APPLY(opacity) \
     APPLY(textAlign) \
     APPLY(textOverflow) \
     APPLY(textTransform) \
     APPLY(tintColor) \
     APPLY(transform) \
     APPLY(transformOriginX) \
     APPLY(transformOriginY) \
     APPLY(zIndex)

#define LS_ADD_ENUM(ENUM) ENUM,

enum StyleProperty {
    LS_FOR_EACH_STYLE_PROPERTY(LS_ADD_ENUM)
};

LS_ENUM_STRING_MAPPING(StyleProperty)

template<>
constexpr int32_t Count<StyleProperty>() noexcept {
    // Note: LS_FOR_EACH_STYLE_PROPERTY(LS_ADD_ENUM) ends with ',', so a placeholder 0 is appended and
    //       subtracted from count.
    return internal::CountMacroVariadicArgs<LS_FOR_EACH_STYLE_PROPERTY(LS_ADD_ENUM) 0>() - 1;
}

#undef LS_ADD_ENUM

constexpr const uint32_t StylePropertyMetaGroupExtended = 0;
constexpr const uint32_t StylePropertyMetaGroupYoga = 1u << 0u;
constexpr const uint32_t StylePropertyMetaTypeEnum = 1u << 1u;
constexpr const uint32_t StylePropertyMetaTypeNumber = 1u << 2u;
constexpr const uint32_t StylePropertyMetaTypeString = 1u << 3u;
constexpr const uint32_t StylePropertyMetaTypeTransform = 1u << 4u;
constexpr const uint32_t StylePropertyMetaTypeColor = 1u << 5u;
constexpr const uint32_t StylePropertyMetaTypeInteger = 1u << 6u;

constexpr const uint32_t kStylePropertyMetaTypeMask = 0b0111'1110;

// StyleProperty enum has enough room to attached meta data in the id, but StyleProperty must be 0 based and
// contiguous. So, the meta data is associated with properties through this array. Each index maps to a StyleProperty
// enum value.

constexpr const uint32_t kStylePropertyMeta[] = {
    /*alignItems*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeEnum,
    /*alignContent*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeEnum,
    /*alignSelf*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeEnum,
    /*border*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*borderBottom*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*borderLeft*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*borderRight*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*borderTop*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*bottom*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*display*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeEnum,
    /*flex*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*flexBasis*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*flexDirection*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeEnum,
    /*flexGrow*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*flexShrink*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*flexWrap*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeEnum,
    /*height*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*justifyContent*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeEnum,
    /*left*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*margin*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*marginBottom*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*marginLeft*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*marginRight*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*marginTop*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*maxHeight*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*maxWidth*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*minHeight*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*minWidth*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*overflow*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeEnum,
    /*padding*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*paddingBottom*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*paddingLeft*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*paddingRight*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*paddingTop*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*position*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeEnum,
    /*right*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*top*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*width*/ StylePropertyMetaGroupYoga | StylePropertyMetaTypeNumber,
    /*backgroundClip*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeEnum,
    /*backgroundColor*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeColor,
    /*backgroundHeight*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*backgroundImage*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeString,
    /*backgroundPositionX*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*backgroundPositionY*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*backgroundRepeat*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeEnum,
    /*backgroundSize*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeEnum,
    /*backgroundWidth*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*borderColor*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeColor,
    /*borderRadius*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*borderRadiusTopLeft*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*borderRadiusTopRight*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*borderRadiusBottomLeft*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*borderRadiusBottomRight*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*color*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeColor,
    /*fontFamily*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeString,
    /*fontSize*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*fontStyle*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeEnum,
    /*fontWeight*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeEnum,
    /*lineHeight*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*maxLines*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeInteger,
    /*objectFit*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeEnum,
    /*objectPositionX*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*objectPositionY*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*opacity*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*textAlign*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeEnum,
    /*textOverflow*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeEnum,
    /*textTransform*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeEnum,
    /*tintColor*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeColor,
    /*transform*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeTransform,
    /*transformOriginX*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*transformOriginY*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeNumber,
    /*zIndex*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeInteger,
};

static_assert(Count<StyleProperty>() == (sizeof(kStylePropertyMeta) / sizeof(int32_t)),
              "StyleProperty and StylePropertyMeta are out of sync.");

constexpr uint32_t StylePropertyMetaGetType(StyleProperty property) noexcept {
    return kStylePropertyMeta[property] & kStylePropertyMetaTypeMask;
}

constexpr bool IsYogaProperty(StyleProperty property) noexcept {
    return kStylePropertyMeta[property] & StylePropertyMetaGroupYoga;
}

void StylePropertyValueInit();
bool StylePropertyValueIsValid(StyleProperty property, int32_t value) noexcept;
const char* StylePropertyValueToString(StyleProperty property, int32_t value) noexcept;
const char* StylePropertyValueDefault(StyleProperty property) noexcept;
int32_t StylePropertyValueFromString(StyleProperty property, const char* value) noexcept;

} // namespace lse

namespace std {
template<>
struct hash<lse::StyleProperty> {
    std::size_t operator()(lse::StyleProperty value) const noexcept {
        return static_cast<std::size_t>(value);
    }
};
} // namespace std
