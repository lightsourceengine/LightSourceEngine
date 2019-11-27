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
#include <ls/Matrix.h>
#include "StyleEnums.h"
#include "StyleValue.h"

namespace ls {

template<typename S /* Scene */, int32_t P = 50>
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
            switch (objectPosition.AsInt32()) {
                case StyleAnchorRight:
                case StyleAnchorBottom:
                    return boxDimension - fitDimension;
                case StyleAnchorCenter:
                    return boxDimension * .5f - fitDimension * .5f;
                default:
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
            return boxDimension * (P / 100.f) - fitDimension * (P / 100.f);
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
float ComputeFontSize(const StyleValueNumber& value, const S* scene, const float rootFontSize) {
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
            return value.value * rootFontSize;
        default:
            return 16.f;
    }
}

template<typename S /* Scene */>
float ComputeFontSize(const StyleValueNumber& value, const S* scene) {
    return ComputeFontSize(value, scene, scene->GetRootFontSize());
}

template<typename I /* ImageResource */, typename S /* Scene */>
Rect ComputeBackgroundImageRect(const StyleValueNumber& backgroundX, const StyleValueNumber& backgroundY,
        const StyleValueNumber& backgroundWidth, const StyleValueNumber& backgroundHeight,
        StyleBackgroundSize backgroundSize, const Rect& bounds, I* imageResource, const S* scene) {
    auto computeDimension = [scene](const StyleValueNumber& backgroundDimension, float boundsDimension,
            float imageDimension) {
        switch (backgroundDimension.unit) {
            case StyleNumberUnitPercent:
                return backgroundDimension.AsPercent() * boundsDimension;
            case StyleNumberUnitPoint:
                return backgroundDimension.value;
            case StyleNumberUnitViewportWidth:
                return backgroundDimension.AsPercent() * scene->GetWidth();
            case StyleNumberUnitViewportHeight:
                return backgroundDimension.AsPercent() * scene->GetHeight();
            case StyleNumberUnitViewportMin:
                return backgroundDimension.AsPercent() * scene->GetViewportMin();
            case StyleNumberUnitViewportMax:
                return backgroundDimension.AsPercent() * scene->GetViewportMax();
            case StyleNumberUnitRootEm:
                return backgroundDimension.value * scene->GetRootFontSize();
            default: // StyleNumberUnitAuto
                return imageDimension;
        }
    };
    float width;
    float height;

    if (imageResource->HasCapInsets()) {
        backgroundSize = StyleBackgroundSizeNone;
    }

    switch (backgroundSize) {
        case StyleBackgroundSizeContain:
            if (imageResource->GetAspectRatio() > (bounds.width / bounds.height)) {
                width = bounds.width;
                height = bounds.width / imageResource->GetAspectRatio();
            } else {
                width = bounds.height * imageResource->GetAspectRatio();
                height = bounds.height;
            }
            break;
        case StyleBackgroundSizeCover:
            if (imageResource->GetAspectRatio() > (bounds.width / bounds.height)) {
                width = bounds.height * imageResource->GetAspectRatio();
                height = bounds.height;
            } else {
                width = bounds.width;
                height = bounds.width / imageResource->GetAspectRatio();
            }
            break;
        default:
            if (imageResource->HasCapInsets()) {
                width = imageResource->GetWidthF();
                height = imageResource->GetHeightF();
            } else {
                width = computeDimension(backgroundWidth, bounds.width, imageResource->GetWidthF());
                height = computeDimension(backgroundHeight, bounds.height, imageResource->GetHeightF());
            }
            break;
    }

    return {
        ComputeObjectPosition<S, 0>(backgroundX, bounds.width, width, scene),
        ComputeObjectPosition<S, 0>(backgroundY, bounds.height, height, scene),
        width,
        height
    };
}

template<typename S /* Scene */>
float ComputeTransformOrigin(const StyleValueNumber& transformOrigin, const float dimension, const S* scene) {
    switch (transformOrigin.unit) {
        case StyleNumberUnitPoint:
            return transformOrigin.value;
        case StyleNumberUnitPercent:
            return transformOrigin.AsPercent() * dimension;
        case StyleNumberUnitAnchor:
        {
            switch (transformOrigin.AsInt32()) {
                case StyleAnchorRight:
                case StyleAnchorBottom:
                    return dimension;
                case StyleAnchorCenter:
                    return dimension * .5f;
                default:
                    return 0;
            }
        }
        case StyleNumberUnitViewportWidth:
            return transformOrigin.AsPercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return transformOrigin.AsPercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return transformOrigin.AsPercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return transformOrigin.AsPercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return transformOrigin.AsPercent() * scene->GetRootFontSize();
        default:
            return dimension * .5f;
    }
}

template<typename S /* Scene */>
Matrix ComputeTransform(const Matrix& base, const StyleValueTransform& transform,
        const StyleValueNumber& transformOriginX, const StyleValueNumber& transformOriginY,
        const Rect& bounds, const S* scene) {
    const auto x{ ComputeTransformOrigin(transformOriginX, bounds.width, scene) };
    const auto y{ ComputeTransformOrigin(transformOriginY, bounds.height, scene) };

    return base * Matrix::Translate(x, y) * transform.ToMatrix(bounds.width, bounds.height) * Matrix::Translate(-x, -y);
}

template<typename S /* Scene */>
float ComputeBorderWidth(const StyleValueNumber& borderValue, float defaultValue, const S* scene) {
    switch (borderValue.unit) {
        case StyleNumberUnitPoint:
            return borderValue.value;
        case StyleNumberUnitViewportWidth:
            return borderValue.AsPercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return borderValue.AsPercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return borderValue.AsPercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return borderValue.AsPercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return borderValue.AsPercent() * scene->GetRootFontSize();
        default:
            return defaultValue;
    }
}

} // namespace ls
