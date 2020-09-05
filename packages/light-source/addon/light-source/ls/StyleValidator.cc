/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/StyleValidator.h>

#include <ls/StyleProperty.h>

namespace ls {

StyleValueValidator StyleValidator::numberValidators[Count<StyleProperty>()]{};

void StyleValidator::Init() {
    auto gte0 = [](float value, StyleNumberUnit) { return value >= 0; };
    auto any = [](float value, StyleNumberUnit) { return true; };
    auto xanchor = [](float value, StyleNumberUnit unit) {
        switch (unit) {
            case StyleNumberUnitAnchor:
                switch (static_cast<int32_t>(value)) {
                    case StyleAnchorLeft:
                    case StyleAnchorRight:
                    case StyleAnchorCenter:
                        return true;
                    default:
                        return false;
                }
                break;
            default:
                return true;
        }
    };
    auto yanchor = [](float value, StyleNumberUnit unit) {
        switch (unit) {
            case StyleNumberUnitAnchor:
                switch (static_cast<int32_t>(value)) {
                    case StyleAnchorTop:
                    case StyleAnchorBottom:
                    case StyleAnchorCenter:
                        return true;
                    default:
                        return false;
                }
                break;
            default:
                return true;
        }
    };
    auto opacity = [](float value, StyleNumberUnit unit) {
        switch (unit) {
            case StyleNumberUnitPoint:
                return value >= 0 && value <= 1;
            case StyleNumberUnitPercent:
                return value >= 0 && value <= 100;
            default:
                return false;
        }
    };
    auto hasPoint = [](StyleNumberUnit unit) {
      switch (unit) {
          case StyleNumberUnitPoint:
          case StyleNumberUnitViewportWidth:
          case StyleNumberUnitViewportHeight:
          case StyleNumberUnitViewportMin:
          case StyleNumberUnitViewportMax:
          case StyleNumberUnitRootEm:
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
    StyleValueValidator validate_xanchor_point_percent_auto_anchor{ xanchor,
        { hasPoint, hasPercent, hasAuto, hasAnchor } };
    StyleValueValidator validate_yanchor_point_percent_auto_anchor{ yanchor,
        { hasPoint, hasPercent, hasAuto, hasAnchor } };

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

    numberValidators[StyleProperty::backgroundPositionX] = validate_xanchor_point_percent_auto_anchor;
    numberValidators[StyleProperty::backgroundPositionY] = validate_yanchor_point_percent_auto_anchor;
    numberValidators[StyleProperty::objectPositionX] = validate_xanchor_point_percent_auto_anchor;
    numberValidators[StyleProperty::objectPositionY] = validate_yanchor_point_percent_auto_anchor;
    numberValidators[StyleProperty::transformOriginX] = validate_xanchor_point_percent_auto_anchor;
    numberValidators[StyleProperty::transformOriginY] = validate_yanchor_point_percent_auto_anchor;

    numberValidators[StyleProperty::fontSize] = validate_gte0_point;
    numberValidators[StyleProperty::lineHeight] = validate_gte0_point_percent;

    numberValidators[StyleProperty::opacity] = { opacity, { hasStrictPoint, hasPercent } };
}

bool StyleValidator::IsValidValue(StyleProperty property, const char* value) noexcept {
    return StylePropertyValueFromString(property, value) >= 0;
}

bool StyleValidator::IsValidValue(StyleProperty property, const StyleValue& value) noexcept {
    assert(numberValidators[property]);
    return numberValidators[property].IsValid(value);
}

bool StyleValidator::IsValidValue(StyleProperty property, int32_t value) noexcept {
    assert(StylePropertyMetaGetType(property) == StylePropertyMetaTypeInteger);

    switch (property) {
        case StyleProperty::maxLines:
            return value >= 0;
        case StyleProperty::zIndex:
            return true;
        default:
            assert(false);
            return false;
    }
}

StyleValueValidator::StyleValueValidator(IsValueOkFunc isValueOk,
        const std::initializer_list<IsUnitSupportedFunc>& isUnitSupportedChecks) noexcept {
    assert(isUnitSupportedChecks.size() > 0 && isUnitSupportedChecks.size() < MAX_UNIT_CHECKS);

    this->isValueOk = isValueOk;

    auto i = 0;

    for (const auto& func : isUnitSupportedChecks) {
        this->isUnitSupported[i++] = func;
    }

    this->isUnitSupportedSize = i;
}

bool StyleValueValidator::IsValid(const StyleValue& styleValue) const noexcept {
    if (this->isValueOk(styleValue.value, styleValue.unit)) {
        for (int32_t i = 0; i < this->isUnitSupportedSize; i++) {
            if (this->isUnitSupported[i](styleValue.unit)) {
                return true;
            }
        }
    }

    return false;
}

} // namespace ls
