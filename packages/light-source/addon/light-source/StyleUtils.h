/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <string>
#include <ls/Size.h>
#include "StyleEnums.h"
#include "StyleValue.h"

namespace ls {

std::string CreateRoundedRectangleUri(const int32_t radiusTopLeft, const int32_t radiusTopRight,
    const int32_t radiusBottomRight, const int32_t radiusBottomLeft, const int32_t stroke);

template<typename S /* Scene */>
float CalculateBackgroundDimension(const StyleNumberValue* styleDimension, const float imageDimension,
                                   const float boxDimension, const S* scene) {
    if (!styleDimension) {
        return imageDimension;
    }

    switch (styleDimension->GetUnit()) {
        case StyleNumberUnitPercent:
            return styleDimension->GetValuePercent() * boxDimension;
        case StyleNumberUnitPoint:
            return styleDimension->GetValue();
        case StyleNumberUnitViewportWidth:
            return styleDimension->GetValuePercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return styleDimension->GetValuePercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return styleDimension->GetValuePercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return styleDimension->GetValuePercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return styleDimension->GetValue() * scene->GetRootFontSize();
        default: // StyleNumberUnitAuto
            return imageDimension;
    }
}

template<typename I /* ImageResource */>
Size CalculateObjectFitDimensions(const StyleObjectFit objectFit, const I* image,
        const float boxWidth, const float boxHeight) {
    if (image->HasCapInsets()) {
        return {
            boxWidth,
            boxHeight
        };
    }

    const auto imageWidth{ static_cast<float>(image->GetWidth()) };
    const auto imageHeight{ static_cast<float>(image->GetHeight()) };
    const auto imageAspectRatio{ imageWidth / imageHeight };

    switch (objectFit) {
        case StyleObjectFitContain:
            if (imageAspectRatio > (boxWidth / boxHeight)) {
                return {
                    boxWidth,
                    boxWidth / imageAspectRatio
                };
            } else {
                return {
                    boxHeight * imageAspectRatio,
                    boxHeight
                };
            }
        case StyleObjectFitCover:
            if (imageAspectRatio > (boxWidth / boxHeight)) {
                return {
                    boxHeight * imageAspectRatio,
                    boxHeight
                };
            } else {
                return {
                    boxWidth,
                    boxWidth / imageAspectRatio
                };
            }
        case StyleObjectFitScaleDown:
            if (imageWidth > boxWidth || imageHeight > boxHeight) {
                // contain
                if (imageAspectRatio > (boxWidth / boxHeight)) {
                    return {
                        boxWidth,
                        boxWidth / imageAspectRatio
                    };
                } else {
                    return {
                        boxHeight * imageAspectRatio,
                        boxHeight
                    };
                }
            } else {
                // none
                return {
                    imageWidth,
                    imageHeight
                };
            }
        case StyleObjectFitNone:
            return {
                imageWidth,
                imageHeight
            };
        default:
            return {
                boxWidth,
                boxHeight
            };
    }
}

template<typename S /* Scene */>
float CalculateObjectPosition(const StyleNumberValue* objectPosition, const bool isX, const float boxDimension,
                              const float fitDimension, const float defaultPercent, const S* scene) {
    if (objectPosition) {
        switch (objectPosition->GetUnit()) {
            case StyleNumberUnitPoint:
                return objectPosition->GetValue();
            case StyleNumberUnitPercent:
            {
                auto percent{ objectPosition->GetValuePercent() };

                return (boxDimension * percent - fitDimension * percent);
            }
            case StyleNumberUnitAnchor:
            {
                if (objectPosition->Int32Value() == (isX ? StyleAnchorRight : StyleAnchorBottom)) {
                    return boxDimension - fitDimension;
                }
                return 0;
            }
            case StyleNumberUnitViewportWidth:
                return objectPosition->GetValuePercent() * scene->GetWidth();
            case StyleNumberUnitViewportHeight:
                return objectPosition->GetValuePercent() * scene->GetHeight();
            case StyleNumberUnitViewportMin:
                return objectPosition->GetValuePercent() * scene->GetViewportMin();
            case StyleNumberUnitViewportMax:
                return objectPosition->GetValuePercent() * scene->GetViewportMax();
            case StyleNumberUnitRootEm:
                return objectPosition->GetValue() * scene->GetRootFontSize();
            default:
                break;
        }
    }

    // default position of objectPosition = 50%
    return boxDimension * defaultPercent - fitDimension * defaultPercent;
}

template<typename S /* Scene */>
int32_t ComputeIntegerPointValue(const StyleNumberValue* styleValue, const S* scene, const int32_t defaultValue) {
    if (!styleValue) {
        return defaultValue;
    }

    switch (styleValue->GetUnit()) {
        case StyleNumberUnitPoint:
            return styleValue->GetValue();
        case StyleNumberUnitViewportWidth:
            return styleValue->GetValuePercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return styleValue->GetValuePercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return styleValue->GetValuePercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return styleValue->GetValuePercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return styleValue->GetValue() * scene->GetRootFontSize();
        default:
            return defaultValue;
    }
}

template<typename S /* Scene */>
float CalculateLineHeight(const StyleNumberValue* styleValue, const S* scene, const float fontLineHeight) {
    if (!styleValue) {
        return fontLineHeight;
    }

    switch (styleValue->GetUnit()) {
        case StyleNumberUnitPoint:
            return styleValue->GetValue();
        case StyleNumberUnitPercent:
            return styleValue->GetValuePercent() * fontLineHeight;
        case StyleNumberUnitViewportWidth:
            return styleValue->GetValuePercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return styleValue->GetValuePercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return styleValue->GetValuePercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return styleValue->GetValuePercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return styleValue->GetValue() * scene->GetRootFontSize();
        default:
            return fontLineHeight;
    }
}

} // namespace ls
