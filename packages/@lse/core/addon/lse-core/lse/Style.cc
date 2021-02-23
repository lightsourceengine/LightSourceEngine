/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/Style.h>

#include <lse/Math.h>
#include <algorithm>
#include <std17/algorithm>
#include <cassert>

namespace lse {

Style Style::empty{};
Style::StylePropertySet Style::tempDefinedProperties;

bool operator==(const StyleValue& a, const StyleValue& b) noexcept {
  switch (a.unit) {
    case StyleNumberUnitUndefined:
    case StyleNumberUnitAuto:
      return a.unit == b.unit;
    default:
      return a.unit == b.unit && Equals(a.value, b.value);
  }
}

bool operator!=(const StyleValue& a, const StyleValue& b) noexcept {
  return !(a == b);
}

bool operator==(const StyleTransformSpec& a, const StyleTransformSpec& b) noexcept {
  switch (a.transform) {
    case StyleTransformIdentity:
      return a.transform == b.transform;
    case StyleTransformRotate:
      return a.transform == b.transform && a.angle == b.angle;
    default:
      return a.transform == b.transform && a.x == b.x && a.y == b.y;
  }
}

bool operator!=(const StyleTransformSpec& a, const StyleTransformSpec& b) noexcept {
  return !(a == b);
}

bool operator==(const std::vector<StyleTransformSpec>& a, const std::vector<StyleTransformSpec>& b) noexcept {
  return std::equal(a.begin(), a.end(), b.begin());
}

bool operator!=(const std::vector<StyleTransformSpec>& a, const std::vector<StyleTransformSpec>& b) noexcept {
  return !(a == b);
}

Style::~Style() {
  if (this->parent) {
    this->parent->Unref();
  }
}

void Style::SetEnum(StyleProperty property, const char* value) {
  int32_t intValue = StylePropertyValueFromString(property, value);

  assert(intValue >= 0);

  if (intValue < 0) {
    return;
  }

  if (!this->enumMap.contains(property) || this->enumMap[property] != intValue) {
    this->enumMap[property] = intValue;

    if (this->onChange) {
      this->onChange(property);
    }
  }
}

const char* Style::GetEnumString(StyleProperty property) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeEnum);

  if (this->enumMap.contains(property)) {
    return StylePropertyValueToString(property, this->enumMap[property]);
  } else if (this->parent) {
    return this->parent->GetEnumString(property);
  } else {
    return StylePropertyValueDefault(property);
  }
}

int32_t Style::GetEnum(StyleProperty property) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeEnum);

  if (this->enumMap.contains(property)) {
    return this->enumMap[property];
  } else if (this->parent) {
    return this->parent->GetEnum(property);
  } else {
    return 0;
  }
}

void Style::SetColor(StyleProperty property, color_t value) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeColor);

  if (!this->colorMap.contains(property) || this->colorMap[property] != value) {
    this->colorMap[property] = value;

    if (this->onChange) {
      this->onChange(property);
    }
  }
}

std17::optional<color_t> Style::GetColor(StyleProperty property) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeColor);

  if (this->colorMap.contains(property)) {
    return this->colorMap[property];
  } else if (this->parent) {
    return this->parent->GetColor(property);
  } else {
    return {};
  }
}

void Style::SetString(StyleProperty property, std::string&& value) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeString);

  if (!this->stringMap.contains(property) || this->stringMap[property] != value) {
    this->stringMap[property] = std::move(value);

    if (this->onChange) {
      this->onChange(property);
    }
  }
}

const std::string& Style::GetString(StyleProperty property) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeString);

  if (this->stringMap.contains(property)) {
    return this->stringMap[property];
  } else if (this->parent) {
    return this->parent->GetString(property);
  } else {
    static const std::string emptyString;
    return emptyString;
  }
}

void Style::SetNumber(StyleProperty property, const StyleValue& value) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeNumber);
  assert(!value.IsUndefined());

  if (value.IsUndefined()) {
    return;
  }

  if (!this->numberMap.contains(property) || this->numberMap[property] != value) {
    this->numberMap[property] = value;

    if (this->onChange) {
      this->onChange(property);
    }
  }
}

const StyleValue& Style::GetNumber(StyleProperty property) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeNumber);

  if (this->numberMap.contains(property)) {
    return this->numberMap[property];
  } else if (this->parent) {
    return this->parent->GetNumber(property);
  } else {
    static const StyleValue emptyStyleValue{};
    return emptyStyleValue;
  }
}

std17::optional<int32_t> Style::GetInteger(StyleProperty property) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeInteger);

  if (this->enumMap.contains(property)) {
    return this->enumMap[property];
  } else if (this->parent) {
    return this->parent->GetInteger(property);
  } else {
    return {};
  }
}

void Style::SetInteger(StyleProperty property, int32_t value) {
  assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeInteger);

  if (!this->enumMap.contains(property) || this->enumMap[property] != value) {
    this->enumMap[property] = value;

    if (this->onChange) {
      this->onChange(property);
    }
  }
}

void Style::SetTransform(std::vector<StyleTransformSpec>&& value) {
  if (this->transform.empty() || this->transform != value) {
    this->transform = std::move(value);

    if (this->onChange) {
      this->onChange(StyleProperty::transform);
    }
  }
}

const std::vector<StyleTransformSpec>& Style::GetTransform() {
  if (!this->transform.empty()) {
    return this->transform;
  } else if (this->parent) {
    return this->parent->transform;
  } else {
    return this->transform;
  }
}

void Style::SetUndefined(StyleProperty property) {
  size_t count;

  switch (StylePropertyMetaGetType(property)) {
    case StylePropertyMetaTypeEnum:
    case StylePropertyMetaTypeInteger:
      count = this->enumMap.erase(property);
      break;
    case StylePropertyMetaTypeString:
      count = this->stringMap.erase(property);
      break;
    case StylePropertyMetaTypeColor:
      count = this->colorMap.erase(property);
      break;
    case StylePropertyMetaTypeNumber:
      count = this->numberMap.erase(property);
      break;
    case StylePropertyMetaTypeTransform:
      count = this->transform.size();
      this->transform.clear();
      break;
    default:
      assert(false);
      break;
  }

  if (count > 0UL && this->onChange) {
    this->onChange(property);
  }
}

bool Style::IsEmpty(StyleProperty property) const noexcept {
  return this->IsEmpty(property, true);
}

bool Style::IsEmpty(StyleProperty property, bool includeParent) const noexcept {
  bool childEmpty;

  switch (StylePropertyMetaGetType(property)) {
    case StylePropertyMetaTypeEnum:
    case StylePropertyMetaTypeInteger:
      childEmpty = !this->enumMap.contains(property);
      break;
    case StylePropertyMetaTypeString:
      childEmpty = !this->stringMap.contains(property);
      break;
    case StylePropertyMetaTypeColor:
      childEmpty = !this->colorMap.contains(property);
      break;
    case StylePropertyMetaTypeNumber:
      childEmpty = !this->numberMap.contains(property);
      break;
    case StylePropertyMetaTypeTransform:
      childEmpty = this->transform.empty();
      break;
    default:
      assert(false);
      return true;
  }

  if (includeParent && this->parent) {
    return childEmpty && this->parent->IsEmpty(property);
  }

  return childEmpty;
}

void Style::SetChangeListener(std::function<void(StyleProperty)>&& changeListener) noexcept {
  this->onChange = std::move(changeListener);
}

void Style::ClearChangeListener() noexcept {
  this->onChange = nullptr;
}

void Style::SetParent(Style* parent) noexcept {
  assert(tempDefinedProperties.empty());

  if (this->onChange) {
    if (parent) {
      parent->GatherDefinedProperties(tempDefinedProperties);
    }

    if (this->parent) {
      this->parent->GatherDefinedProperties(tempDefinedProperties);
    }
  }

  if (this->parent) {
    this->parent->Unref();
  }

  this->parent = parent;

  if (this->parent) {
    this->parent->Ref();
  }

  for (auto property : tempDefinedProperties) {
    // TODO: check exists/isempty
    this->onChange(property);
  }

  tempDefinedProperties.clear();
}

Style* Style::Or(Style* style) noexcept {
  if (style) {
    return style;
  } else {
    return &empty;
  }
}

void Style::OnMediaChange(bool remChange, bool viewportChange) noexcept {
  if (!this->onChange) {
    return;
  }

  for (const auto& entry : this->numberMap) {
    switch (entry.second.unit) {
      case StyleNumberUnitRootEm:
        if (remChange) {
          this->onChange(entry.first);
        }
        break;
      case StyleNumberUnitViewportWidth:
      case StyleNumberUnitViewportHeight:
      case StyleNumberUnitViewportMin:
      case StyleNumberUnitViewportMax:
        if (viewportChange) {
          this->onChange(entry.first);
        }
        break;
      default:
        break;
    }
  }
}

void Style::Reset() {
  assert(tempDefinedProperties.empty());

  if (this->onChange) {
    this->GatherDefinedProperties(tempDefinedProperties);
  }

  this->enumMap.clear();
  this->numberMap.clear();
  this->colorMap.clear();
  this->stringMap.clear();
  this->transform.clear();

  for (auto property : tempDefinedProperties) {
    if (this->parent && !this->parent->IsEmpty(property, false)) {
      // TODO: send defined flag
      this->onChange(property);
    } else {
      // TODO: send undefined flag
      this->onChange(property);
    }
  }

  tempDefinedProperties.clear();
}

void Style::GatherDefinedProperties(StylePropertySet& properties) {
  for (const auto& p : this->enumMap) {
    properties.insert(p.first);
  }

  for (const auto& p : this->numberMap) {
    properties.insert(p.first);
  }

  for (const auto& p : this->colorMap) {
    properties.insert(p.first);
  }

  for (const auto& p : this->stringMap) {
    properties.insert(p.first);
  }

  if (!this->transform.empty()) {
    properties.insert(StyleProperty::transform);
  }
}

void Style::Init() {
  tempDefinedProperties.reserve(Count<StyleProperty>());
}

} // namespace lse
