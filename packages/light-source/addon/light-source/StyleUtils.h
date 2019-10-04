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
                                   const float imageDimension,
                                   const float boxDimension,
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
        case StyleNumberUnitRootEm:
            return styleDimension->GetValue() * scene->GetRootFontSize();
        default: // StyleNumberUnitAuto
            return imageDimension;
    }
}

inline
YGSize CalculateObjectFitDimensions(const StyleObjectFit objectFit,
                                  const ImageResource* image,
                                  const float boxWidth,
                                  const float boxHeight) {
    const auto imageWidth{ static_cast<float>(image->GetWidth()) };
    const auto imageHeight{ static_cast<float>(image->GetHeight()) };
    const auto imageAspectRatio{ imageWidth / imageHeight };

    switch (image->HasCapInsets() ? StyleObjectFitFill : objectFit) {
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
            case StyleNumberUnitRootEm:
                return objectPosition->GetValue() * scene->GetRootFontSize();
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
        case StyleNumberUnitRootEm:
            return styleValue->GetValue() * scene->GetRootFontSize();
        default:
            return defaultValue;
    }
}

inline
float CalculateLineHeight(const StyleNumberValue* styleValue, const Scene* scene, const float fontLineHeight) {
    switch (styleValue ? styleValue->GetUnit() : StyleNumberUnitAuto) {
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

inline
std::string CreateRoundedRectangleUri(const int32_t radiusTopLeft,
                                      const int32_t radiusTopRight,
                                      const int32_t radiusBottomRight,
                                      const int32_t radiusBottomLeft,
                                      const int32_t stroke) {
    static const std::string empty{};
    static const std::string uriTemplate{
        "data:image/svg+xml,<svg viewBox=\"0 0 {0} {1}\">"
            "<path d=\"M {2},0 h{3} {4} v{5} {6} h-{7} {8} v-{9} {10} z\" "
                "fill=\"{11}\" "
                "stroke=\"{12}\" "
                "stroke-width=\"{13}\"/>"
        "</svg>"
    };

    const auto leftWidth{ std::max(radiusTopLeft, radiusBottomLeft) };
    const auto rightWidth{ std::max(radiusTopRight, radiusBottomRight) };
    const auto width{ leftWidth + rightWidth + 1 };

    const auto topHeight{ std::max(radiusTopLeft, radiusTopRight) };
    const auto bottomHeight{ std::max(radiusBottomLeft, radiusBottomRight) };
    const auto height{ topHeight + bottomHeight + 1 };

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
