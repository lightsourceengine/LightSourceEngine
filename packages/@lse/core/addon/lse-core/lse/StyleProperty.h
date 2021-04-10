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
#include <functional>

namespace lse {

#define LSE_FOR_EACH_STYLE_PROPERTY(APPLY) \
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
     APPLY(color) \
     APPLY(filter) \
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
     APPLY(transform) \
     APPLY(transformOriginX) \
     APPLY(transformOriginY) \
     APPLY(zIndex)

#define LSE_ADD_ENUM(ENUM) ENUM,

enum StyleProperty {
  LSE_FOR_EACH_STYLE_PROPERTY(LSE_ADD_ENUM)
};

LSE_ENUM_STRING_MAPPING(StyleProperty)

template<>
constexpr int32_t Count<StyleProperty>() noexcept {
  // Note: LSE_FOR_EACH_STYLE_PROPERTY(LSE_ADD_ENUM) ends with ',', so a placeholder 0 is appended and
  //       subtracted from count.
  return internal::CountMacroVariadicArgs<LSE_FOR_EACH_STYLE_PROPERTY(LSE_ADD_ENUM) 0>() - 1;
}

#undef LSE_ADD_ENUM

constexpr const uint32_t StylePropertyMetaGroupExtended = 0;
constexpr const uint32_t StylePropertyMetaGroupYoga = 1u << 0u;

constexpr const uint32_t StylePropertyMetaTypeEnum = 1u << 1u;
constexpr const uint32_t StylePropertyMetaTypeNumber = 1u << 2u;
constexpr const uint32_t StylePropertyMetaTypeString = 1u << 3u;
constexpr const uint32_t StylePropertyMetaTypeTransform = 1u << 4u;
constexpr const uint32_t StylePropertyMetaTypeColor = 1u << 5u;
constexpr const uint32_t StylePropertyMetaTypeInteger = 1u << 6u;
constexpr const uint32_t StylePropertyMetaTypeFilter = 1u << 7u;

constexpr const uint32_t kStylePropertyMetaTypeMask = 0b1111'1110;

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
    /*color*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeColor,
    /*filter*/ StylePropertyMetaGroupExtended | StylePropertyMetaTypeFilter,
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
