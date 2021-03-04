/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/Reference.h>
#include <lse/StyleEnums.h>
#include <lse/Color.h>
#include <lse/math-ext.h>
#include <Yoga.h>
#include <phmap.h>
#include <phmap_utils.h>
#include <std17/optional>

namespace lse {

/**
 * Number Style property value. Wraps a float value with a unit type.
 */
struct StyleValue {
  float value{ kUndefined };
  StyleNumberUnit unit{ StyleNumberUnitUndefined };

  StyleValue() = default;

  StyleValue(float value, StyleNumberUnit unit) : value(value), unit(unit) {
    if (std::isnan(value)) {
      this->value = kUndefined;
      this->unit = StyleNumberUnitUndefined;
    }
  }

  StyleValue(float value, int32_t unit) : value(value) {
    if (!IsEnum<StyleNumberUnit>(unit) || std::isnan(value)) {
      this->value = kUndefined;
      this->unit = StyleNumberUnitUndefined;
    }

    this->unit = static_cast<StyleNumberUnit>(unit);
  }

  bool IsUndefined() const noexcept {
    return this->unit == StyleNumberUnitUndefined;
  }

  bool IsAuto() const noexcept {
    return this->unit == StyleNumberUnitAuto;
  }

  static StyleValue OfAuto() noexcept {
    return { 0, StyleNumberUnitAuto };
  }

  static StyleValue OfUndefined() noexcept {
    return {};
  }

  static StyleValue OfPoint(float value) noexcept {
    return { value, StyleNumberUnitPoint };
  }

  static StyleValue OfAnchor(StyleAnchor anchor) noexcept {
    return { static_cast<float>(anchor), StyleNumberUnitAnchor };
  }
};

bool operator==(const StyleValue& a, const StyleValue& b) noexcept;
bool operator!=(const StyleValue& a, const StyleValue& b) noexcept;

/**
 * Entry in transform Style property array representing specific transforms of identity, rotate, scale and translate.
 */
struct StyleTransformSpec {
  StyleTransform transform{};
  StyleValue x{};
  StyleValue y{};
  StyleValue angle{};

  static StyleTransformSpec OfIdentity() {
    return { StyleTransformIdentity, {}, {}, {}};
  }

  static StyleTransformSpec OfScale(const StyleValue& x, const StyleValue& y) {
    return { StyleTransformScale, x, y, {}};
  }

  static StyleTransformSpec OfTranslate(const StyleValue& x, const StyleValue& y) {
    return { StyleTransformTranslate, x, y, {}};
  }

  static StyleTransformSpec OfRotate(const StyleValue& angle) {
    return { StyleTransformRotate, {}, {}, angle };
  }
};

bool operator==(const StyleTransformSpec& a, const StyleTransformSpec& b) noexcept;
bool operator!=(const StyleTransformSpec& a, const StyleTransformSpec& b) noexcept;

struct StyleFilterFunction {
  StyleFilter filter{};
  color_t color{};
};

bool operator==(const StyleFilterFunction& a, const StyleFilterFunction& b) noexcept;
bool operator!=(const StyleFilterFunction& a, const StyleFilterFunction& b) noexcept;

class Style : public Reference {
 public:
  ~Style() override;

  // enum based properties
  void SetEnum(StyleProperty property, const char* value);
  const char* GetEnumString(StyleProperty property);
  int32_t GetEnum(StyleProperty property);
  template<typename T>
  T GetEnum(StyleProperty property) {
    return static_cast<T>(this->GetEnum(property));
  }

  // color based properties
  void SetColor(StyleProperty property, color_t color);
  std17::optional<color_t> GetColor(StyleProperty property);

  // string based properties
  void SetString(StyleProperty property, std::string&& value);
  const std::string& GetString(StyleProperty property);

  // number or StyleValue based properties
  void SetNumber(StyleProperty property, const StyleValue& value);
  const StyleValue& GetNumber(StyleProperty property);

  // integer based properties
  std17::optional<int32_t> GetInteger(StyleProperty property);
  void SetInteger(StyleProperty property, int32_t value);

  // transform property
  void SetTransform(std::vector<StyleTransformSpec>&& transform);
  const std::vector<StyleTransformSpec>& GetTransform();

  // filter property
  void SetFilter(std::vector<StyleFilterFunction>&& filter);
  const std::vector<StyleFilterFunction>& GetFilter();

  // property existence: these function will check the parent!
  bool IsEmpty(StyleProperty property) const noexcept;
  bool Exists(StyleProperty property) const noexcept { return !this->IsEmpty(property); }

  // Clear a property.
  void SetUndefined(StyleProperty property);

  // Change listener. Called when a property value changes (set undefined or set to new value)
  void SetChangeListener(std::function<void(StyleProperty)>&& changeListener) noexcept;
  void ClearChangeListener() noexcept;

  // Set the parent. If a value in this style is undefined, the parent will be queried.
  void SetParent(Style* parent) noexcept;

  // Handle root font size and viewport changes. The method will force a change on properties that use rem, vw, vmin,
  // etc units.
  void OnMediaChange(bool remChange, bool viewportChange) noexcept;

  void Reset();

  void Lock() noexcept { this->locked = true; }
  bool IsLocked() const noexcept { return this->locked; }

  static void Init();
  static Style* Or(Style* style) noexcept;

 private:
  using StylePropertySet = phmap::flat_hash_set<StyleProperty>;

  void GatherDefinedProperties(StylePropertySet& properties);
  bool IsEmpty(StyleProperty property, bool includeParent) const noexcept;

 private:
  // Style state

  bool locked{false};
  Style* parent{};
  std::function<void(StyleProperty)> onChange;

  // Property buckets

  phmap::flat_hash_map<StyleProperty, color_t> colorMap;
  phmap::flat_hash_map<StyleProperty, std::string> stringMap;
  phmap::flat_hash_map<StyleProperty, StyleValue> numberMap;
  phmap::flat_hash_map<StyleProperty, int32_t> enumMap;
  std::vector<StyleTransformSpec> transform;
  std::vector<StyleFilterFunction> filter;

  // Static helpers

  static StylePropertySet tempDefinedProperties;
  static Style empty;
};

} // namespace lse
