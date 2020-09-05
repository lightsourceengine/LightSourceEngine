/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/StyleEnums.h>
#include <ls/types.h>
#include <ls/Color.h>
#include <ls/Math.h>
#include <Yoga.h>
#include <phmap.h>
#include <std17/optional>

namespace ls {

/**
 * Number Style property value. Wraps a float value with a unit type.
 */
struct StyleValue {
    StyleNumberUnit unit{ StyleNumberUnitUndefined };
    float value{ kUndefined };

    bool IsUndefined() const noexcept {
        return this->unit == StyleNumberUnitUndefined;
    }

    bool IsAuto() const noexcept {
        return this->unit == StyleNumberUnitAuto;
    }

    static StyleValue OfAuto() noexcept {
        return { StyleNumberUnitAuto, 0 };
    }

    static StyleValue OfUndefined() noexcept {
        return {};
    }

    static StyleValue OfPoint(float value) noexcept {
        return { StyleNumberUnitPoint, value };
    }

    static StyleValue OfAnchor(StyleAnchor anchor) noexcept {
        return { StyleNumberUnitAnchor, static_cast<float>(anchor) };
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
        return { StyleTransformIdentity, {}, {}, {} };
    }

    static StyleTransformSpec OfScale(const StyleValue& x, const StyleValue& y) {
        return { StyleTransformScale, x, y, {} };
    }

    static StyleTransformSpec OfTranslate(const StyleValue& x, const StyleValue& y) {
        return { StyleTransformTranslate, x, y, {} };
    }

    static StyleTransformSpec OfRotate(const StyleValue& angle) {
        return { StyleTransformRotate, {}, {}, angle };
    }
};

bool operator==(const StyleTransformSpec& a, const StyleTransformSpec& b) noexcept;
bool operator!=(const StyleTransformSpec& a, const StyleTransformSpec& b) noexcept;

class Style {
 public:
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

    // property existence: these function will check the parent!
    bool IsEmpty(StyleProperty property) const noexcept;
    bool Exists(StyleProperty property) const noexcept { return !this->IsEmpty(property); }

    // Clear a property.
    void SetUndefined(StyleProperty property);

    // Change listener. Called when a property value changes (set undefined or set to new value)
    void SetChangeListener(std::function<void(StyleProperty)>&& changeListener) noexcept;
    void ClearChangeListener() noexcept;

    // Set the parent. If a value in this style is undefined, the parent will be queried.
    void SetParent(const StyleRef& parent) noexcept;

    // Handle root font size and viewport changes. The method will force a change on properties that use rem, vw, vmin,
    // etc units.
    void OnMediaChange(bool remChange, bool viewportChange) noexcept;

    void Reset();

    static void Init();
    static Style* Or(Style* style) noexcept;
    static Style* Or(const StyleRef& style) noexcept;

 private:
    void GatherDefinedProperties(phmap::flat_hash_set<StyleProperty>& properties);
    bool IsEmpty(StyleProperty property, bool includeParent) const noexcept;

 private:
    // Property buckets

    phmap::flat_hash_map<StyleProperty, color_t> colorMap;
    phmap::flat_hash_map<StyleProperty, std::string> stringMap;
    phmap::flat_hash_map<StyleProperty, StyleValue> numberMap;
    phmap::flat_hash_map<StyleProperty, int32_t> enumMap;
    std::vector<StyleTransformSpec> transform;

    // Style state

    StyleRef parent;
    std::function<void(StyleProperty)> onChange;

    // Static helpers

    static phmap::flat_hash_set<StyleProperty> tempDefinedProperties;
    static Style empty;
};

}; // namespace ls
