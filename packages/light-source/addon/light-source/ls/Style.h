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
        return { StyleNumberUnitPoint, static_cast<float>(anchor) };
    }
};

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

/**
 * Validator for Style property numbers.
 *
 * The isValueOk checks the value field of a StyleValue number. If isValueOk returns false, the StyleValue fails
 * validation. If the value passes, each isUnitSupported function is called until at least 1 function returns true
 * for the StyleValue unit field.
 */
struct StyleValueValidator {
    static constexpr auto MAX_UNIT_CHECKS = 6;

    using IsValueOkFunc = bool(*)(float);
    using IsUnitSupportedFunc = bool(*)(StyleNumberUnit);

    IsValueOkFunc isValueOk{};
    IsUnitSupportedFunc isUnitSupported[MAX_UNIT_CHECKS]{};
    int32_t isUnitSupportedSize{};

    StyleValueValidator() noexcept = default;
    StyleValueValidator(IsValueOkFunc isValueOk,
        const std::initializer_list<IsUnitSupportedFunc>& isUnitSupportedChecks) noexcept;

    bool IsValid(const StyleValue& styleValue) const noexcept;

    explicit operator bool() const noexcept { return this->isValueOk; }
};

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

    void ForEachYogaProperty(const std::function<void(StyleProperty)>& func);
    void Reset();

    static void Init();
    static void InitValidators();
    static Style* Or(Style* style) noexcept;
    static Style* Or(const StyleRef& style) noexcept;

 private:
    // Structure that type-erases enum string mapping functions.
    struct EnumOps {
        using IsValid = bool(*)(int32_t);
        using ToString = const char*(*)(int32_t);
        using FromString = int32_t(*)(const char*);

        IsValid isValid{};
        ToString toString{};
        FromString fromString{};

        template<typename T>
        static EnumOps Of() {
            return { IsValidEnum<T>, StylePropertyIntToString<T>, StylePropertyIntFromString<T> };
        }

        explicit operator bool() const noexcept { return this->isValid && this->fromString && this->toString; }
    };

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

    static Style empty;
    static std::vector<StyleProperty> yogaProperties;
    static EnumOps enumOps[];
    static StyleValueValidator numberValidators[];
};

}; // namespace ls
