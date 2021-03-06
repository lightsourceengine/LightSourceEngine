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

#include <lse/StyleEnums.h>

#include <lse/string-ext.h>
#include <stdexcept>

namespace lse {

template<typename T>
CStringHashMap<T> LoadFromStringMap() {
  CStringHashMap<T> map;

  for (int32_t i = 0; i < Count<T>(); i++) {
    auto e{ static_cast<T>(i) };

    map.insert(std::make_pair(ToString(e), e));
  }

  return map;
}

static auto sStyleFontStyleFromString = LoadFromStringMap<StyleFontStyle>();
static auto sStyleFontWeightFromString = LoadFromStringMap<StyleFontWeight>();
static auto sStyleTextOverflowFromString = LoadFromStringMap<StyleTextOverflow>();
static auto sStyleTextAlignFromString = LoadFromStringMap<StyleTextAlign>();
static auto sStyleBackgroundClipFromString = LoadFromStringMap<StyleBackgroundClip>();
static auto sStyleTextTransformFromString = LoadFromStringMap<StyleTextTransform>();
static auto sStyleAnchorFromString = LoadFromStringMap<StyleAnchor>();
static auto sStyleBackgroundRepeatFromString = LoadFromStringMap<StyleBackgroundRepeat>();
static auto sStyleBackgroundSizeFromString = LoadFromStringMap<StyleBackgroundSize>();
static auto sStyleObjectFitFromString = LoadFromStringMap<StyleObjectFit>();
static auto sYGAlignFromString = LoadFromStringMap<YGAlign>();
static auto sYGDisplayFromString = LoadFromStringMap<YGDisplay>();
static auto sYGFlexDirectionFromString = LoadFromStringMap<YGFlexDirection>();
static auto sYGJustifyFromString = LoadFromStringMap<YGJustify>();
static auto sYGOverflowFromString = LoadFromStringMap<YGOverflow>();
static auto sYGWrapFromString = LoadFromStringMap<YGWrap>();
static auto sYGPositionTypeFromString = LoadFromStringMap<YGPositionType>();

const char* StyleFontWeightToString(const StyleFontWeight value) noexcept {
  switch (value) {
    case StyleFontWeightNormal:
      return "normal";
    case StyleFontWeightBold:
      return "bold";
  }
  return "unknown";
}

const char* StyleFontStyleToString(const StyleFontStyle value) noexcept {
  switch (value) {
    case StyleFontStyleNormal:
      return "normal";
    case StyleFontStyleItalic:
      return "italic";
    case StyleFontStyleOblique:
      return "oblique";
  }
  return "unknown";
}

const char* ToString(const StyleFontStyle value) noexcept {
  return StyleFontStyleToString(value);
}

const char* StyleTextOverflowToString(const StyleTextOverflow value) noexcept {
  switch (value) {
    case StyleTextOverflowNone:
      return "none";
    case StyleTextOverflowClip:
      return "clip";
    case StyleTextOverflowEllipsis:
      return "ellipsis";
  }
  return "unknown";
}

const char* StyleTextAlignToString(const StyleTextAlign value) noexcept {
  switch (value) {
    case StyleTextAlignLeft:
      return "left";
    case StyleTextAlignCenter:
      return "center";
    case StyleTextAlignRight:
      return "right";
  }
  return "unknown";
}

const char* StyleObjectFitToString(const StyleObjectFit value) noexcept {
  switch (value) {
    case StyleObjectFitFill:
      return "fill";
    case StyleObjectFitContain:
      return "contain";
    case StyleObjectFitCover:
      return "cover";
    case StyleObjectFitNone:
      return "none";
    case StyleObjectFitScaleDown:
      return "scale-down";
  }
  return "unknown";
}

const char* StyleBackgroundClipToString(const StyleBackgroundClip value) noexcept {
  switch (value) {
    case StyleBackgroundClipBorderBox:
      return "border-box";
    case StyleBackgroundClipPaddingBox:
      return "padding-box";
    case StyleBackgroundClipContentBox:
      return "content-box";
  }
  return "unknown";
}

const char* StyleBackgroundRepeatToString(const StyleBackgroundRepeat value) noexcept {
  switch (value) {
    case StyleBackgroundRepeatXY:
      return "repeat";
    case StyleBackgroundRepeatX:
      return "repeat-x";
    case StyleBackgroundRepeatY:
      return "repeat-y";
    case StyleBackgroundRepeatOff:
      return "no-repeat";
  }
  return "unknown";
}

const char* StyleBackgroundSizeToString(const StyleBackgroundSize value) noexcept {
  switch (value) {
    case StyleBackgroundSizeNone:
      return "none";
    case StyleBackgroundSizeCover:
      return "cover";
    case StyleBackgroundSizeContain:
      return "contain";
  }
  return "unknown";
}

const char* StyleTextTransformToString(const StyleTextTransform value) noexcept {
  switch (value) {
    case StyleTextTransformNone:
      return "none";
    case StyleTextTransformUppercase:
      return "uppercase";
    case StyleTextTransformLowercase:
      return "lowercase";
  }
  return "unknown";
}

const char* StyleAnchorToString(const StyleAnchor value) noexcept {
  switch (value) {
    case StyleAnchorTop:
      return "top";
    case StyleAnchorRight:
      return "right";
    case StyleAnchorBottom:
      return "bottom";
    case StyleAnchorLeft:
      return "left";
    case StyleAnchorCenter:
      return "center";
  }
  return "unknown";
}

template<typename T>
T StyleEnumFromString(const CStringHashMap<T>& map, const char* value) {
  auto it{ value ? map.find(value) : map.end() };

  if (it == map.end()) {
    throw std::invalid_argument(value ? value : "null");
  }

  return it->second;
}

StyleFontStyle StyleFontStyleFromString(const char* value) {
  return StyleEnumFromString(sStyleFontStyleFromString, value);
}

StyleFontWeight StyleFontWeightFromString(const char* value) {
  return StyleEnumFromString(sStyleFontWeightFromString, value);
}

StyleTextAlign StyleTextAlignFromString(const char* value) {
  return StyleEnumFromString(sStyleTextAlignFromString, value);
}

StyleTextOverflow StyleTextOverflowFromString(const char* value) {
  return StyleEnumFromString(sStyleTextOverflowFromString, value);
}

StyleBackgroundClip StyleBackgroundClipFromString(const char* value) {
  return StyleEnumFromString(sStyleBackgroundClipFromString, value);
}

StyleTextTransform StyleTextTransformFromString(const char* value) {
  return StyleEnumFromString(sStyleTextTransformFromString, value);
}

StyleAnchor StyleAnchorFromString(const char* value) {
  return StyleEnumFromString(sStyleAnchorFromString, value);
}

StyleBackgroundRepeat StyleBackgroundRepeatFromString(const char* value) {
  return StyleEnumFromString(sStyleBackgroundRepeatFromString, value);
}

StyleBackgroundSize StyleBackgroundSizeFromString(const char* value) {
  return StyleEnumFromString(sStyleBackgroundSizeFromString, value);
}

StyleObjectFit StyleObjectFitFromString(const char* value) {
  return StyleEnumFromString(sStyleObjectFitFromString, value);
}

YGAlign YGAlignFromString(const char* value) {
  return StyleEnumFromString(sYGAlignFromString, value);
}

YGDisplay YGDisplayFromString(const char* value) {
  return StyleEnumFromString(sYGDisplayFromString, value);
}

YGFlexDirection YGFlexDirectionFromString(const char* value) {
  return StyleEnumFromString(sYGFlexDirectionFromString, value);
}

YGJustify YGJustifyFromString(const char* value) {
  return StyleEnumFromString(sYGJustifyFromString, value);
}

YGOverflow YGOverflowFromString(const char* value) {
  return StyleEnumFromString(sYGOverflowFromString, value);
}

YGWrap YGWrapFromString(const char* value) {
  return StyleEnumFromString(sYGWrapFromString, value);
}

YGPositionType YGPositionTypeFromString(const char* value) {
  return StyleEnumFromString(sYGPositionTypeFromString, value);
}

const char* StylePropertyToString(StyleProperty property) noexcept {
#define TO_STRING(PROP) case StyleProperty::PROP: return #PROP;

  switch (property) {
    LSE_FOR_EACH_STYLE_PROPERTY(TO_STRING)
  }

#undef TO_STRING

  return "unknown";
}

} // namespace lse
