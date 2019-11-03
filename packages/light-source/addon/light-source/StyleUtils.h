/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <string>
#include <ls/Size.h>
#include <ls/Rect.h>
#include <ls/Math.h>
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

template<typename S /* Scene */>
float ComputeObjectPosition(const StyleNumberValue* objectPosition, const float boxDimension,
                            const float fitDimension, const S* scene) {
    if (objectPosition) {
        switch (objectPosition->GetUnit()) {
            case StyleNumberUnitPoint:
                return objectPosition->GetValue();
            case StyleNumberUnitPercent:
            {
                const auto percent{ objectPosition->GetValuePercent() };

                return (boxDimension * percent - fitDimension * percent);
            }
            case StyleNumberUnitAnchor:
            {
                const auto anchor{ objectPosition->Int32Value() };

                if (anchor == StyleAnchorRight || anchor == StyleAnchorBottom) {
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

    return boxDimension * 0.5f - fitDimension * 0.5f;
}

template<typename I /* ImageResource */, typename S /* Scene */>
Rect ComputeObjectFitRect(StyleObjectFit objectFit, const StyleNumberValue* objectPositionX,
        const StyleNumberValue* objectPositionY, const Rect& bounds, const I* image, const S* scene) noexcept {
    if (image->HasCapInsets()) {
        objectFit = StyleObjectFitFill;
    } else if (objectFit == StyleObjectFitScaleDown) {
        if (image->GetWidthF() > bounds.width || image->GetHeightF() > bounds.height) {
            objectFit = StyleObjectFitContain;
        } else {
            objectFit = StyleObjectFitNone;
        }
    }

    float fitWidth;
    float fitHeight;

    switch (objectFit) {
        case StyleObjectFitContain:
            if (image->GetAspectRatio() > (bounds.width / bounds.height)) {
                fitWidth = bounds.width;
                fitHeight = bounds.width / image->GetAspectRatio();
            } else {
                fitWidth = bounds.height * image->GetAspectRatio();
                fitHeight = bounds.height;
            }
            break;
        case StyleObjectFitCover:
            if (image->GetAspectRatio() > (bounds.width / bounds.height)) {
                fitWidth = bounds.height * image->GetAspectRatio();
                fitHeight = bounds.height;
            } else {
                fitWidth = bounds.width;
                fitHeight = bounds.width / image->GetAspectRatio();
            }
            break;
        case StyleObjectFitNone:
            fitWidth = image->GetWidthF();
            fitHeight = image->GetHeightF();
            break;
        default:
            fitWidth = bounds.width;
            fitHeight = bounds.height;
            break;
    }

    return {
        SnapToPixelGrid(bounds.x + ComputeObjectPosition(objectPositionX, bounds.width, fitWidth, scene)),
        SnapToPixelGrid(bounds.y + ComputeObjectPosition(objectPositionY, bounds.height, fitHeight, scene)),
        SnapToPixelGrid(fitWidth),
        SnapToPixelGrid(fitHeight)
    };
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
float ComputeLineHeight(const StyleNumberValue* styleValue, const S* scene, const float fontLineHeight) {
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

template<typename S /* Scene */>
float ComputeFontSize(const StyleNumberValue* styleValue, const S* scene) {
    if (!styleValue) {
        // TODO: default value?
        return 16.f;
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
            return 16.f;
    }
}

} // namespace ls
