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

#include <lse/StyleProperty.h>
#include <lse/StyleEnums.h>
#include <cassert>
#include <stdexcept>
#include <YGStyle.h>

namespace lse {

template<typename T>
static const char* StylePropertyIntToString(int32_t property) noexcept {
  return IsValidEnum<T>(property) ? ToString<T>(static_cast<T>(property)) : "unknown";
}

template<typename T>
static int32_t StylePropertyIntFromString(const char* value) {
  return FromString<T>(value);
}

struct EnumOps {
  using IsValid = bool (*)(int32_t);
  using ToString = const char* (*)(int32_t);
  using FromString = int32_t(*)(const char*);

  IsValid isValid{};
  ToString toString{};
  FromString fromString{};
  int32_t defaultValue{};

  template<typename T>
  static EnumOps Of(int32_t defaultValue = 0) noexcept {
    return { IsValidEnum<T>, StylePropertyIntToString<T>, StylePropertyIntFromString<T>, defaultValue };
  }

  explicit operator bool() const noexcept { return this->isValid && this->fromString && this->toString; }
};

EnumOps enumOps[Count<StyleProperty>()]{};

void StylePropertyValueInit() {
  YGStyle ygStyleDefaults{};

  enumOps[StyleProperty::alignItems] = EnumOps::Of<YGAlign>(ygStyleDefaults.alignItems());
  enumOps[StyleProperty::alignContent] = EnumOps::Of<YGAlign>(ygStyleDefaults.alignContent());
  enumOps[StyleProperty::alignSelf] = EnumOps::Of<YGAlign>(ygStyleDefaults.alignSelf());
  enumOps[StyleProperty::display] = EnumOps::Of<YGDisplay>(ygStyleDefaults.display());
  enumOps[StyleProperty::flexDirection] = EnumOps::Of<YGFlexDirection>(ygStyleDefaults.flexDirection());
  enumOps[StyleProperty::flexWrap] = EnumOps::Of<YGWrap>(ygStyleDefaults.flexWrap());
  enumOps[StyleProperty::justifyContent] = EnumOps::Of<YGJustify>(ygStyleDefaults.justifyContent());
  enumOps[StyleProperty::overflow] = EnumOps::Of<YGOverflow>(ygStyleDefaults.overflow());
  enumOps[StyleProperty::position] = EnumOps::Of<YGPositionType>(ygStyleDefaults.positionType());

  enumOps[StyleProperty::backgroundClip] = EnumOps::Of<StyleBackgroundClip>();
  enumOps[StyleProperty::backgroundRepeat] = EnumOps::Of<StyleBackgroundRepeat>();
  enumOps[StyleProperty::backgroundSize] = EnumOps::Of<StyleBackgroundSize>();
  enumOps[StyleProperty::fontStyle] = EnumOps::Of<StyleFontStyle>();
  enumOps[StyleProperty::fontWeight] = EnumOps::Of<StyleFontWeight>();
  enumOps[StyleProperty::objectFit] = EnumOps::Of<StyleObjectFit>();
  enumOps[StyleProperty::textAlign] = EnumOps::Of<StyleTextAlign>();
  enumOps[StyleProperty::textOverflow] = EnumOps::Of<StyleTextOverflow>();
  enumOps[StyleProperty::textTransform] = EnumOps::Of<StyleTextTransform>();

  for (size_t i = 0; i < Count<StyleProperty>(); i++) {
    if (StylePropertyMetaGetType(static_cast<StyleProperty>(i)) == StylePropertyMetaTypeEnum) {
      assert(enumOps[i]);
    } else {
      assert(!enumOps[i]);
    }
  }
}

bool StylePropertyValueIsValid(StyleProperty property, int32_t value) noexcept {
  assert(enumOps[property]);
  return enumOps[property].isValid(value);
}

const char* StylePropertyValueToString(StyleProperty property, int32_t value) noexcept {
  assert(enumOps[property]);
  return enumOps[property].toString(value);
}

int32_t StylePropertyValueFromString(StyleProperty property, const char* value) noexcept {
  assert(enumOps[property]);
  try {
    return enumOps[property].fromString(value);
  } catch (std::invalid_argument&) {
    return -1;
  }
}

const char* StylePropertyValueDefault(StyleProperty property) noexcept {
  assert(enumOps[property]);
  return StylePropertyValueToString(property, enumOps[property].defaultValue);
}

} // namespace lse
