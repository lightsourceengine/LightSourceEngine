/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <Yoga.h>
#include "StyleEnums.h"

namespace lse {

void YGStyleNullSetAuto(const YGNodeRef node);
void YGStyleNullSetAuto(const YGNodeRef node, const YGEdge);
void YGStyleNullSetValue(const YGNodeRef node, const float);
void YGStyleNullSetValue(const YGNodeRef node, const YGEdge, const float);

class StyleValue {
 public:
    virtual ~StyleValue() = default;

    virtual Napi::Value ToJS(Napi::Env env) const = 0;
    virtual void Apply(const YGNodeRef ygNode, const float viewportWidth, const float viewportHeight) const { }
};

template<typename E>
class StyleEnumValue : public StyleValue {
 public:
    explicit StyleEnumValue(E value) : value(value) {}
    virtual ~StyleEnumValue() = default;

    E Get() const {
        return this->value;
    }

    void Set(E e) {
        this->value = e;
    }

    virtual Napi::Value ToJS(Napi::Env env) const {
        return Napi::Number::New(env, this->value);
    }

    static bool ToValue(Napi::Value value, E* result) {
        if (value.IsNumber()) {
            auto num = value.As<Napi::Number>().Int32Value();

            if (IsEnum<E>(num)) {
                *result = static_cast<E>(num);
                return true;
            }
        }

        return false;
    }

 private:
    E value{};
};

class StyleColorValue : public StyleValue {
 public:
    explicit StyleColorValue(const int64_t value) : value(value) {}
    virtual ~StyleColorValue() = default;

    void Set(const int64_t val) {
        this->value = val;
    }

    int64_t Get() const {
        return this->value;
    }

    uint32_t Value() const {
        return static_cast<uint32_t>(this->value & 0xFFFFFFFF);
    }

    uint8_t Alpha() const {
        return (this->value >> 24) & 0xFF;
    }

    virtual Napi::Value ToJS(Napi::Env env) const {
        return Napi::Number::New(env, this->value);
    }

    static bool ToValue(Napi::Value value, int64_t* result);

    // TODO: rgb
    // TODO: endianess

 private:
    int64_t value{};
};

struct StyleNumber {
    StyleNumberUnit unit{};
    float value{};
};

class StyleNumberValue : public StyleValue {
 public:
    explicit StyleNumberValue(const StyleNumber& number) : number(number) {}
    virtual ~StyleNumberValue() = default;

    StyleNumberUnit GetUnit() const { return this->number.unit; }

    float GetValue() const { return this->number.value; }

    float GetValuePercent() const { return this->number.value / 100.f; }

    int32_t Int32Value() const { return static_cast<int32_t>(this->number.value); }

    void Set(const StyleNumber& number) { this->number = number; }

    Napi::Value ToJS(Napi::Env env) const;

    static bool ToValue(Napi::Value value, StyleNumber* result);

 protected:
    StyleNumber number{};
};

template<typename E, void (*S)(const YGNodeRef, const E)>
class YogaStyleEnumValue : public StyleEnumValue<E> {
 public:
    explicit YogaStyleEnumValue(E value) : StyleEnumValue<E>(value) {}
    virtual ~YogaStyleEnumValue() = default;

    virtual void Apply(const YGNodeRef ygNode, const float viewportWidth, const float viewportHeight) const {
        S(ygNode, this->Get());
    }
};

template<void (*S)(const YGNodeRef, const float),
         void (*P)(const YGNodeRef, const float) = YGStyleNullSetValue,
         void (*A)(const YGNodeRef) = YGStyleNullSetAuto>
class YogaStyleNumberValue : public StyleNumberValue {
 public:
    explicit YogaStyleNumberValue(const StyleNumber& number) : StyleNumberValue(number) {}
    virtual ~YogaStyleNumberValue() = default;

    virtual void Apply(const YGNodeRef ygNode, const float viewportWidth, const float viewportHeight) const {
        switch (this->number.unit) {
            case StyleNumberUnitAuto:
                A(ygNode);
                break;
            case StyleNumberUnitPercent:
                P(ygNode, this->number.value);
                break;
            case StyleNumberUnitPoint:
                S(ygNode, this->number.value);
                break;
            case StyleNumberUnitViewportWidth:
                S(ygNode, this->GetValuePercent() * viewportWidth);
                break;
            case StyleNumberUnitViewportHeight:
                S(ygNode, this->GetValuePercent() * viewportHeight);
                break;
            case StyleNumberUnitViewportMin:
                S(ygNode, this->GetValuePercent() * viewportWidth > viewportHeight ? viewportHeight : viewportWidth);
                break;
            case StyleNumberUnitViewportMax:
                S(ygNode, this->GetValuePercent() * viewportWidth > viewportHeight ? viewportWidth : viewportHeight);
                break;
            default:
                break;
        }
    }
};

template<YGEdge edge,
         void (*S)(const YGNodeRef, const YGEdge, const float),
         void (*P)(const YGNodeRef, const YGEdge, const float) = YGStyleNullSetValue,
         void (*A)(const YGNodeRef, const YGEdge) = YGStyleNullSetAuto>
class YogaEdgeStyleNumberValue : public StyleNumberValue {
 public:
    explicit YogaEdgeStyleNumberValue(const StyleNumber& number) : StyleNumberValue(number) {}
    virtual ~YogaEdgeStyleNumberValue() = default;

    virtual void Apply(const YGNodeRef ygNode, const float viewportWidth, const float viewportHeight) const {
        switch (this->number.unit) {
            case StyleNumberUnitAuto:
                A(ygNode, edge);
                break;
            case StyleNumberUnitPercent:
                P(ygNode, edge, this->number.value);
                break;
            case StyleNumberUnitPoint:
                S(ygNode, edge, this->number.value);
                break;
            case StyleNumberUnitViewportWidth:
                S(ygNode, edge, this->GetValuePercent() * viewportWidth);
                break;
            case StyleNumberUnitViewportHeight:
                S(ygNode, edge, this->GetValuePercent() * viewportHeight);
                break;
            case StyleNumberUnitViewportMin:
                S(ygNode,
                  edge,
                  this->GetValuePercent() * viewportWidth > viewportHeight ? viewportHeight : viewportWidth);
                break;
            case StyleNumberUnitViewportMax:
                S(ygNode,
                  edge,
                  this->GetValuePercent() * viewportWidth > viewportHeight ? viewportWidth : viewportHeight);
                break;
            default:
                break;
        }
    }
};

} // namespace lse