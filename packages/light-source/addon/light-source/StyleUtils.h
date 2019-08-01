/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "StyleEnums.h"
#include "Scene.h"
#include "ImageResource.h"
#include <algorithm>
#include <fmt/format.h>

namespace ls {

inline
float CalculateBackgroundDimension(const StyleNumberValue* styleDimension,
                                   float imageDimension,
                                   float boxDimension,
                                   const Scene* scene) {
    switch (styleDimension ? styleDimension->GetUnit() : StyleNumberUnitAuto) {
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
        default: // StyleNumberUnitAuto
            return imageDimension;
    }
}

inline
void CalculateObjectFitDimensions(StyleObjectFit objectFit,
                                  ImageResource* image,
                                  float boxWidth,
                                  float boxHeight,
                                  float* fitWidth,
                                  float* fitHeight) {
    auto imageWidth{ static_cast<float>(image->GetWidth()) };
    auto imageHeight{ static_cast<float>(image->GetHeight()) };
    auto imageAspectRatio{ imageWidth / imageHeight };

    *fitWidth = boxWidth;
    *fitHeight = boxHeight;

    switch (objectFit) {
        case StyleObjectFitContain:
            if (imageAspectRatio > (boxWidth / boxHeight)) {
                *fitHeight = boxWidth / imageAspectRatio;
            } else {
                *fitWidth = boxHeight * imageAspectRatio;
            }
            break;
        case StyleObjectFitCover:
            if (imageAspectRatio > (boxWidth / boxHeight)) {
                *fitWidth = boxHeight * imageAspectRatio;
            } else {
                *fitHeight = boxWidth / imageAspectRatio;
            }
            break;
        case StyleObjectFitScaleDown:
            if (imageWidth > boxWidth || imageHeight > boxHeight) {
                // contain
                if (imageAspectRatio > (boxWidth / boxHeight)) {
                    *fitHeight = boxWidth / imageAspectRatio;
                } else {
                    *fitWidth = boxHeight * imageAspectRatio;
                }
            } else {
                // none
                *fitWidth = imageWidth;
                *fitHeight = imageHeight;
            }
            break;
        case StyleObjectFitNone:
            *fitWidth = imageWidth;
            *fitHeight = imageHeight;
        break;
        case StyleObjectFitFill:
//            fitWidth = boxWidth;
//            fitHeight = boxHeight;
            break;
    }
}

inline
float CalculateObjectPosition(const StyleNumberValue* objectPosition,
                              const bool isX,
                              const float boxDimension,
                              const float fitDimension,
                              const float defaultPercent,
                              const Scene* scene) {
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
            default:
                break;
        }
    }

    // default position of objectPosition = 50%
    return boxDimension * defaultPercent - fitDimension * defaultPercent;
}

inline
int32_t ComputeIntegerPointValue(const StyleNumberValue* styleValue, const Scene* scene, const int32_t defaultValue) {
    switch (styleValue ? styleValue->GetUnit() : StyleNumberUnitAuto) {
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
        default:
            return defaultValue;
    }
}

inline
std::string CreateRoundedRectangleUri(int32_t radiusTopLeft,
                                      int32_t radiusTopRight,
                                      int32_t radiusBottomRight,
                                      int32_t radiusBottomLeft,
                                      int32_t stroke) {
    static std::string empty{};
    static const auto uriTemplate = "data:image/svg+xml,<svg viewBox=\"0 0 {0} {1}\">"
                               "<path d=\"M {2},0 h{3} {4} v{5} {6} h-{7} {8} v-{9} {10} z\" "
                                   "fill=\"{11}\" "
                                   "stroke=\"{12}\" "
                                   "stroke-width=\"{13}\"/>"
                               "</svg>";

    auto leftWidth = std::max(radiusTopLeft, radiusBottomLeft);
    auto rightWidth = std::max(radiusTopRight, radiusBottomRight);
    auto width = leftWidth + rightWidth + 1;

    auto topHeight = std::max(radiusTopLeft, radiusTopRight);
    auto bottomHeight = std::max(radiusBottomLeft, radiusBottomRight);
    auto height = topHeight + bottomHeight + 1;

    return fmt::format(uriTemplate,
    /* 0: viewbox       */ width,
    /* 1: viewbox       */ height,
    /* 2: M {},0        */ radiusTopLeft,
    /* 3: h             */ (radiusTopLeft == 0 ? leftWidth : 0) + 1 + (radiusTopRight == 0 ? rightWidth : 0),
    /* 4: a             */ radiusTopRight > 0 ? fmt::format("a{0},{0} 0 0 1 {0},{0}", radiusTopRight) : empty,
    /* 5: v             */ (radiusTopRight == 0 ? topHeight : 0) + 1 + (radiusBottomRight == 0 ? bottomHeight : 0),
    /* 6: a             */ radiusBottomRight > 0 ? fmt::format(" a{0},{0} 0 0 1 -{0},{0}", radiusBottomRight) : empty,
    /* 7: h-            */ (radiusBottomLeft == 0 ? leftWidth : 0) + 1 + (radiusBottomRight == 0 ? rightWidth : 0),
    /* 8: a             */ radiusBottomLeft > 0 ? fmt::format("a{0},{0} 0 0 1 -{0},-{0}", radiusBottomLeft) : empty,
    /* 9: v-            */ (radiusTopLeft == 0 ? topHeight : 0) + 1 + (radiusBottomLeft == 0 ? bottomHeight : 0),
    /* 10: a            */ radiusTopLeft > 0 ? fmt::format("a{0},{0} 0 0 1 {0},-{0}", radiusTopLeft) : empty,
    /* 11: fill         */ stroke > 0 ? "none" : "white",
    /* 12: stroke       */ stroke > 0 ? "white" : "none",
    /* 13: stroke-width */ stroke);
}

} // namespace ls
