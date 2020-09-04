/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/Style.h>

#include <ls/Math.h>
#include <std17/algorithm>
#include <cassert>

namespace ls {

Style Style::empty{};
std::vector<StyleProperty> Style::yogaProperties;
Style::EnumOps Style::enumOps[Count<StyleProperty>()]{};
StyleValueValidator Style::numberValidators[Count<StyleProperty>()]{};

void Style::SetEnum(StyleProperty property, const char* value) {
    assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeEnum);
    assert(enumOps[property]);

    int32_t intValue;

    try {
        intValue = enumOps[property].fromString(value);
    } catch (std::invalid_argument& e) {
        // TODO: assert(false);
        intValue = 0;
    }

    // TODO: check if intValue == enumMap[property]

    this->enumMap[property] = intValue;

    if (this->onChange) {
        this->onChange(property);
    }
}

const char* Style::GetEnumString(StyleProperty property) {
    assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeEnum);
    assert(enumOps[property]);

    if (this->enumMap.contains(property)) {
        return enumOps[property].toString(this->enumMap[property]);
    } else if (this->parent) {
        return this->parent->GetEnumString(property);
    } else {
        return enumOps[property].toString(0);
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

void Style::SetColor(StyleProperty property, color_t color) {
    assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeColor);

    bool hasChange;

    if (this->IsEmpty(property)) {
        hasChange = true;
    } else {
        hasChange = this->colorMap[property] != color;
    }

    if (hasChange) {
        this->colorMap[property] = color;

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
    bool hasChange;

    if (this->IsEmpty(property)) {
        hasChange = true;
    } else {
        hasChange = this->stringMap[property] != value;
    }

    if (hasChange) {
        this->stringMap[property] = std::move(value);

        if (this->onChange) {
            this->onChange(property);
        }
    }
}

const std::string& Style::GetString(StyleProperty property) {
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
    bool hasChange;

    if (this->IsEmpty(property)) {
        hasChange = true;
    } else {
        StyleValue& ref = this->numberMap[property];
        hasChange = ref.unit != value.unit || !Equals(ref.value, value.value);
    }

    if (hasChange) {
        this->numberMap[property] = value;

        if (this->onChange) {
            this->onChange(property);
        }
    }
}

const StyleValue& Style::GetNumber(StyleProperty property) {
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
    if (this->enumMap.contains(property)) {
        return this->enumMap[property];
    } else if (this->parent) {
        return this->parent->GetInteger(property);
    }

    return {};
}

void Style::SetInteger(StyleProperty property, int32_t value) {
    assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeInteger);

    this->enumMap[property] = value;
}

void Style::SetTransform(std::vector<StyleTransformSpec>&& transform) {
    this->transform = std::move(transform);

    if (this->onChange) {
        this->onChange(StyleProperty::transform);
    }
}

const std::vector<StyleTransformSpec>& Style::GetTransform() {
    return this->transform;
}

void Style::SetUndefined(StyleProperty property) {
    switch (StylePropertyMetaGetType(property)) {
        case StylePropertyMetaTypeEnum:
        case StylePropertyMetaTypeInteger:
            this->enumMap.erase(property);
            break;
        case StylePropertyMetaTypeString:
            this->stringMap.erase(property);
            break;
        case StylePropertyMetaTypeColor:
            this->colorMap.erase(property);
            break;
        case StylePropertyMetaTypeNumber:
            this->numberMap.erase(property);
            break;
        case StylePropertyMetaTypeTransform:
            this->transform.clear();
            break;
        default:
            assert(false);
            break;
    }
}

bool Style::IsEmpty(StyleProperty property) const noexcept {
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

    if (this->parent) {
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

void Style::SetParent(const StyleRef& parent) noexcept {
    this->parent = parent;
}

Style* Style::Or(Style* style) noexcept {
    if (style) {
        return style;
    } else {
        return &empty;
    }
}

Style* Style::Or(const StyleRef& style) noexcept {
    return Or(style.get());
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
    this->enumMap.clear();
    this->numberMap.clear();
    this->transform.clear();
    this->stringMap.clear();
    this->transform.clear();
}

void Style::ForEachYogaProperty(const std::function<void(StyleProperty)>& func) {
    for (const auto property : yogaProperties) {
        if (!this->IsEmpty(property)) {
            func(property);
        }
    }
}

void Style::Init() {
    size_t size = 0;

    for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
        if (IsYogaProperty(static_cast<StyleProperty>(i))) {
            size++;
        }
    }

    yogaProperties.reserve(size);

    for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
        auto property = static_cast<StyleProperty>(i);

        if (IsYogaProperty(property)) {
            yogaProperties.push_back(property);
        }
    }

    enumOps[StyleProperty::alignItems] = EnumOps::Of<YGAlign>();
    enumOps[StyleProperty::alignContent] = EnumOps::Of<YGAlign>();
    enumOps[StyleProperty::alignSelf] = EnumOps::Of<YGAlign>();
    enumOps[StyleProperty::display] = EnumOps::Of<YGDisplay>();
    enumOps[StyleProperty::flexDirection] = EnumOps::Of<YGFlexDirection>();
    enumOps[StyleProperty::flexWrap] = EnumOps::Of<YGWrap>();
    enumOps[StyleProperty::justifyContent] = EnumOps::Of<YGJustify>();
    enumOps[StyleProperty::overflow] = EnumOps::Of<YGOverflow>();
    enumOps[StyleProperty::position] = EnumOps::Of<YGPositionType>();
    enumOps[StyleProperty::backgroundClip] = EnumOps::Of<StyleBackgroundClip>();
    enumOps[StyleProperty::backgroundRepeat] = EnumOps::Of<StyleBackgroundRepeat>();
    enumOps[StyleProperty::backgroundSize] = EnumOps::Of<StyleBackgroundSize>();
    enumOps[StyleProperty::fontStyle] = EnumOps::Of<StyleFontStyle>();
    enumOps[StyleProperty::fontWeight] = EnumOps::Of<StyleFontWeight>();
    enumOps[StyleProperty::objectFit] = EnumOps::Of<StyleObjectFit>();
    enumOps[StyleProperty::textAlign] = EnumOps::Of<StyleTextAlign>();
    enumOps[StyleProperty::textOverflow] = EnumOps::Of<StyleTextOverflow>();
    enumOps[StyleProperty::textTransform] = EnumOps::Of<StyleTextTransform>();

    InitValidators();
}

void Style::InitValidators() {
    auto gte0 = [](float value) { return value >= 0; };
    auto any = [](float value) { return true; };
    auto hasPoint = [](StyleNumberUnit unit) {
      switch (unit) {
          case StyleNumberUnitPoint:
          case StyleNumberUnitViewportWidth:
          case StyleNumberUnitViewportHeight:
          case StyleNumberUnitViewportMin:
          case StyleNumberUnitViewportMax:
              return true;
          default:
              return false;
      }
    };
    auto hasStrictPoint = [](StyleNumberUnit unit) { return unit == StyleNumberUnitPoint; };
    auto hasPercent = [](StyleNumberUnit unit) { return unit == StyleNumberUnitPercent; };
    auto hasAuto = [](StyleNumberUnit unit) { return unit == StyleNumberUnitAuto; };
    auto hasAnchor = [](StyleNumberUnit unit) { return unit == StyleNumberUnitAnchor; };

    StyleValueValidator validate_gte0_point{ gte0, { hasPoint } };
    StyleValueValidator validate_gte0_point_percent{ gte0, { hasPoint, hasPercent } };
    StyleValueValidator validate_gte0_point_percent_auto{ gte0, { hasPoint, hasPercent, hasAuto } };
    StyleValueValidator validate_any_point_percent{ any, { hasPoint, hasPercent } };
    StyleValueValidator validate_any_point_percent_auto{ any, { hasPoint, hasPercent, hasAuto } };
    StyleValueValidator validate_any_point_percent_auto_anchor{ any, { hasPoint, hasPercent, hasAuto, hasAnchor } };

    numberValidators[StyleProperty::border] = validate_gte0_point;
    numberValidators[StyleProperty::borderBottom] = validate_gte0_point;
    numberValidators[StyleProperty::borderLeft] = validate_gte0_point;
    numberValidators[StyleProperty::borderRight] = validate_gte0_point;
    numberValidators[StyleProperty::borderTop] = validate_gte0_point;

    numberValidators[StyleProperty::borderRadius] = validate_gte0_point_percent;
    numberValidators[StyleProperty::borderRadiusTopLeft] = validate_gte0_point_percent;
    numberValidators[StyleProperty::borderRadiusTopRight] = validate_gte0_point_percent;
    numberValidators[StyleProperty::borderRadiusBottomLeft] = validate_gte0_point_percent;
    numberValidators[StyleProperty::borderRadiusBottomRight] = validate_gte0_point_percent;

    numberValidators[StyleProperty::flexBasis] = validate_gte0_point_percent_auto;

    numberValidators[StyleProperty::flex] = validate_gte0_point;
    numberValidators[StyleProperty::flexGrow] = validate_gte0_point;
    numberValidators[StyleProperty::flexShrink] = validate_gte0_point;

    numberValidators[StyleProperty::width] = validate_gte0_point_percent_auto;
    numberValidators[StyleProperty::height] = validate_gte0_point_percent_auto;
    numberValidators[StyleProperty::backgroundWidth] = validate_gte0_point_percent_auto;
    numberValidators[StyleProperty::backgroundHeight] = validate_gte0_point_percent_auto;

    numberValidators[StyleProperty::maxHeight] = validate_gte0_point_percent;
    numberValidators[StyleProperty::maxWidth] = validate_gte0_point_percent;
    numberValidators[StyleProperty::minHeight] = validate_gte0_point_percent;
    numberValidators[StyleProperty::minWidth] = validate_gte0_point_percent;

    numberValidators[StyleProperty::top] = validate_any_point_percent;
    numberValidators[StyleProperty::right] = validate_any_point_percent;
    numberValidators[StyleProperty::bottom] = validate_any_point_percent;
    numberValidators[StyleProperty::left] = validate_any_point_percent;

    numberValidators[StyleProperty::margin] = validate_any_point_percent_auto;
    numberValidators[StyleProperty::marginBottom] = validate_any_point_percent_auto;
    numberValidators[StyleProperty::marginLeft] = validate_any_point_percent_auto;
    numberValidators[StyleProperty::marginRight] = validate_any_point_percent_auto;
    numberValidators[StyleProperty::marginTop] = validate_any_point_percent_auto;

    numberValidators[StyleProperty::padding] = validate_any_point_percent;
    numberValidators[StyleProperty::paddingBottom] = validate_any_point_percent;
    numberValidators[StyleProperty::paddingLeft] = validate_any_point_percent;
    numberValidators[StyleProperty::paddingRight] = validate_any_point_percent;
    numberValidators[StyleProperty::paddingTop] = validate_any_point_percent;

    numberValidators[StyleProperty::backgroundPositionX] = validate_any_point_percent_auto_anchor;
    numberValidators[StyleProperty::backgroundPositionY] = validate_any_point_percent_auto_anchor;
    numberValidators[StyleProperty::objectPositionX] = validate_any_point_percent_auto_anchor;
    numberValidators[StyleProperty::objectPositionY] = validate_any_point_percent_auto_anchor;
    numberValidators[StyleProperty::transformOriginX] = validate_any_point_percent_auto_anchor;
    numberValidators[StyleProperty::transformOriginY] = validate_any_point_percent_auto_anchor;

    numberValidators[StyleProperty::fontSize] = validate_gte0_point;
    numberValidators[StyleProperty::lineHeight] = validate_gte0_point;

    numberValidators[StyleProperty::opacity] = { gte0, { hasStrictPoint, hasPercent } };
}

StyleValueValidator::StyleValueValidator(IsValueOkFunc isValueOk,
        const std::initializer_list<IsUnitSupportedFunc>& isUnitSupportedChecks) noexcept {
    assert(isUnitSupportedChecks.size() > 0 && isUnitSupportedChecks.size() < MAX_UNIT_CHECKS);

    this->isValueOk = isValueOk;

    auto i = 0;

    for (const auto& func : isUnitSupportedChecks) {
        this->isUnitSupported[i++] = func;
    }
}

bool StyleValueValidator::IsValid(const StyleValue& styleValue) const noexcept {
    if (this->isValueOk(styleValue.value)) {
        for (int32_t i = 0; i < this->isUnitSupportedSize; i++) {
            if (this->isUnitSupported[i](styleValue.unit)) {
                return true;
            }
        }
    }

    return false;
}

} // namespace ls
