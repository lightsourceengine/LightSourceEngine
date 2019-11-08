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
float CalculateBackgroundDimension(const StyleValueNumber& styleDimension, const float imageDimension,
                                   const float boxDimension, const S* scene) {
    switch (styleDimension.unit) {
        case StyleNumberUnitPercent:
            return styleDimension.AsPercent() * boxDimension;
        case StyleNumberUnitPoint:
            return styleDimension.value;
        case StyleNumberUnitViewportWidth:
            return styleDimension.AsPercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return styleDimension.AsPercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return styleDimension.AsPercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return styleDimension.AsPercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return styleDimension.value * scene->GetRootFontSize();
        default: // StyleNumberUnitAuto
            return imageDimension;
    }
}

template<typename S /* Scene */>
float ComputeObjectPosition(const StyleValueNumber& objectPosition, const float boxDimension,
                            const float fitDimension, const S* scene) {
    switch (objectPosition.unit) {
        case StyleNumberUnitPoint:
            return objectPosition.value;
        case StyleNumberUnitPercent:
        {
            const auto percent{ objectPosition.AsPercent() };

            return (boxDimension * percent - fitDimension * percent);
        }
        case StyleNumberUnitAnchor:
        {
            const auto anchor{ objectPosition.AsInt32() };

            if (anchor == StyleAnchorRight || anchor == StyleAnchorBottom) {
                return boxDimension - fitDimension;
            }

            return 0;
        }
        case StyleNumberUnitViewportWidth:
            return objectPosition.AsPercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return objectPosition.AsPercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return objectPosition.AsPercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return objectPosition.AsPercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return objectPosition.AsPercent() * scene->GetRootFontSize();
        default:
            return boxDimension * 0.5f - fitDimension * 0.5f;
    }
}

template<typename I /* ImageResource */, typename S /* Scene */>
Rect ComputeObjectFitRect(StyleObjectFit objectFit, const StyleValueNumber& objectPositionX,
        const StyleValueNumber& objectPositionY, const Rect& bounds, const I* image, const S* scene) noexcept {
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
int32_t ComputeIntegerPointValue(const StyleValueNumber& value, const S* scene, const int32_t defaultValue) {
    switch (value.unit) {
        case StyleNumberUnitPoint:
            return value.value;
        case StyleNumberUnitViewportWidth:
            return value.AsPercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return value.AsPercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return value.AsPercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return value.AsPercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return value.value * scene->GetRootFontSize();
        default:
            return defaultValue;
    }
}

template<typename S /* Scene */>
float ComputeLineHeight(const StyleValueNumber& value, const S* scene, const float fontLineHeight) {
    switch (value.unit) {
        case StyleNumberUnitPoint:
            return value.value;
        case StyleNumberUnitPercent:
            return value.AsPercent() * fontLineHeight;
        case StyleNumberUnitViewportWidth:
            return value.AsPercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return value.AsPercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return value.AsPercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return value.AsPercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return value.value * scene->GetRootFontSize();
        default:
            return fontLineHeight;
    }
}

template<typename S /* Scene */>
float ComputeFontSize(const StyleValueNumber& value, const S* scene) {
    switch (value.unit) {
        case StyleNumberUnitPoint:
            return value.value;
        case StyleNumberUnitViewportWidth:
            return value.AsPercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return value.AsPercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return value.AsPercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return value.AsPercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return value.value * scene->GetRootFontSize();
        default:
            return 16.f;
    }
}

} // namespace ls
