/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "StyleEnums.h"
#include <ls/Math.h>
#include <Yoga.h>
#include <napi.h>
#include <ls/Matrix.h>

namespace ls {

/**
 * 32-bit, RGB color style property.
 */
struct StyleValueColor {
    uint32_t value{};
    bool undefined{true};

    // Binding methods.
    static Napi::Value Box(Napi::Env env, const StyleValueColor& value);
    static StyleValueColor Unbox(const Napi::Value& value);
    static StyleValueColor Parse(const std::string& value) noexcept;

    // Factory methods.
    static StyleValueColor Of(uint32_t value) noexcept { return { value, false }; }
    static StyleValueColor OfUndefined() noexcept { return {}; }

    // operators
    StyleValueColor& operator=(const StyleValueColor&) = default;
    explicit operator uint32_t() const noexcept { return this->value; }
    explicit operator bool() const noexcept { return !this->undefined; }

    // Check if the color value is not defined.
    bool empty() const noexcept { return this->undefined; }

    uint32_t ValueOr(const uint32_t defaultValue) const noexcept {
        return this->undefined ? defaultValue : this->value;
    }
};

/**
 * Style property with a value and a unit.
 */
struct StyleValueNumber {
    StyleNumberUnit unit{StyleNumberUnitUndefined};
    float value{};

    // Binding methods.
    static Napi::Value Box(Napi::Env env, const StyleValueNumber& value);
    static StyleValueNumber Unbox(const Napi::Value& value);

    // Factory methods.
    static StyleValueNumber Of(float value, StyleNumberUnit unit) noexcept { return { unit, value }; }
    static StyleValueNumber OfUndefined() noexcept { return {}; }
    static StyleValueNumber OfPoint(float value) noexcept { return { StyleNumberUnitPoint, value }; }
    static StyleValueNumber OfAuto() noexcept { return { StyleNumberUnitAuto, 0 }; }
    static StyleValueNumber OfAnchor(StyleAnchor anchor) noexcept {
        return { StyleNumberUnitAnchor, static_cast<float>(anchor) };
    }

    // Value conversion methods.
    float AsPercent() const noexcept { return this->value / 100.f; }
    int32_t AsInt32() const noexcept { return static_cast<int32_t>(this->value); }

    // operators
    StyleValueNumber& operator=(const StyleValueNumber&) = default;
    explicit operator int32_t() const { return static_cast<int32_t>(this->value); }
    explicit operator float() const { return this->value; }
    explicit operator bool() const { return this->unit != StyleNumberUnitUndefined; }

    // Check if the number value is not defined.
    bool empty() const noexcept { return this->unit == StyleNumberUnitUndefined; }
};

/**
 * Represents an entry in the transform property array. It specifies the transform type (rotate, scale or translate)
 * and value declaration data.
 */
struct Transform {
    StyleTransform type;
    float value1;
    StyleNumberUnit unit1;
    float value2;
    StyleNumberUnit unit2;
};

/**
 * Value for transform property, an array of transforms.
 */
struct StyleValueTransform {
    std::vector<Transform> values;

    // Binding methods.
    static Napi::Value Box(Napi::Env env, const StyleValueTransform& value);
    static StyleValueTransform Unbox(const Napi::Value& value);

    // operators
    StyleValueTransform& operator=(const StyleValueTransform&) = default;

    // Check if the color value is not defined.
    bool empty() const noexcept { return this->values.empty(); }
};

/**
 * Enum style property.
 *
 * Note, Style stores the enum and uses this structure for the binding methods.
 */
struct StyleValueEnum {
    template<class T>
    static Napi::Value Box(Napi::Env env, const T& value);

    template<class T>
    static T Unbox(const Napi::Value& value);
};

/**
 * String style property.
 *
 * Note, Style stores the string and uses this structure for the binding methods.
 */
struct StyleValueString {
    static Napi::Value Box(Napi::Env env, const std::string& value);
    static std::string Unbox(const Napi::Value& value);
};

struct ObjectPositionConstraint {
    bool operator()(const StyleValueNumber& value) noexcept {
        return value.unit != StyleNumberUnitAuto;
    }
};

struct GTEZeroConstraint {
    bool operator()(const StyleValueNumber& value) noexcept {
        return value.value >= 0 && value.unit != StyleNumberUnitAnchor;
    }
};

struct PointOnlyGTEZeroConstraint {
    bool operator()(const StyleValueNumber& value) noexcept {
        return value.value >= 0 && value.unit != StyleNumberUnitAuto
            && value.unit != StyleNumberUnitAnchor && value.unit != StyleNumberUnitPercent;
    }
};

struct PointPercentOnlyGTEZeroConstraint {
    bool operator()(const StyleValueNumber& value) noexcept {
        return value.value >= 0 && value.unit != StyleNumberUnitAuto && value.unit != StyleNumberUnitAnchor;
    }
};

struct PointPercentOnlyConstraint {
    bool operator()(const StyleValueNumber& value) noexcept {
        return value.unit != StyleNumberUnitAuto && value.unit != StyleNumberUnitAnchor;
    }
};

struct MarginConstraint {
    bool operator()(const StyleValueNumber& value) noexcept {
        return value.unit != StyleNumberUnitAnchor;
    }
};

struct OpacityConstraint {
    bool operator()(const StyleValueNumber& value) {
        switch (value.unit) {
            case StyleNumberUnitPoint:
                return value.value >= 0 && value.value <= 1;
            case StyleNumberUnitPercent:
                return value.value >= 0 && value.value <= 100;
            default:
                return false;
        }
    }
};

inline bool operator==(const StyleValueColor& lhs, const StyleValueColor& rhs) noexcept {
    return lhs.undefined == rhs.undefined && (lhs.undefined || lhs.value == rhs.value);
}

bool operator==(const StyleValueTransform& lhs, const StyleValueTransform& rhs) noexcept;

bool operator==(const Transform& lhs, const Transform& rhs) noexcept;

inline bool operator==(const StyleValueNumber& lhs, const StyleValueNumber& rhs) noexcept {
    return lhs.unit == rhs.unit
        && (lhs.unit == StyleNumberUnitUndefined || lhs.unit == StyleNumberUnitAuto || Equals(lhs.value, rhs.value));
}

template<class T>
Napi::Value StyleValueEnum::Box(Napi::Env env, const T& value) {
    return Napi::String::New(env, ToString(value));
}

template<class T>
T StyleValueEnum::Unbox(const Napi::Value& value) {
    if (value.IsString()) {
        try {
            return FromString<T>(value.As<Napi::String>().Utf8Value().c_str());
        } catch (const std::invalid_argument&) {
            // ignore
        }
    }

    return static_cast<T>(0);
}

} // namespace ls
