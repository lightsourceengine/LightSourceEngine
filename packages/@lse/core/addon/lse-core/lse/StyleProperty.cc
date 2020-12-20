/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/StyleProperty.h>
#include <ls/StyleEnums.h>
#include <cassert>
#include <stdexcept>
#include <YGStyle.h>

namespace ls {

template<typename T>
static const char* StylePropertyIntToString(int32_t property) noexcept {
    return IsValidEnum<T>(property) ? ToString<T>(static_cast<T>(property)) : "unknown";
}

template<typename T>
static int32_t StylePropertyIntFromString(const char* value) {
    return FromString<T>(value);
}

struct EnumOps {
    using IsValid = bool(*)(int32_t);
    using ToString = const char*(*)(int32_t);
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
    } catch (std::invalid_argument& e) {
        return -1;
    }
}

const char* StylePropertyValueDefault(StyleProperty property) noexcept {
    assert(enumOps[property]);
    return StylePropertyValueToString(property, enumOps[property].defaultValue);
}

} // namespace ls
