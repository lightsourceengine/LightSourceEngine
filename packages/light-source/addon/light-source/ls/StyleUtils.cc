/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/StyleUtils.h>

#include <ls/Style.h>
#include <ls/Scene.h>
#include <ls/Resources.h>

namespace ls {

static float ComputeTransformOrigin(Scene* scene, const StyleValueNumber& transformOrigin, float dimension) noexcept;
static float ComputeObjectFitCoordinate(Scene* scene, const StyleValueNumber& coordinate, float boxDimension,
        float fitDimension, float defaultPercent = 0.5f) noexcept;

Matrix ComputeTransform(Scene* scene, Style* style, const Rect& box) noexcept {
    if (!style) {
        return Matrix::Identity();
    }

    const auto x{ ComputeTransformOrigin(scene, style->transformOriginX, box.width) };
    const auto y{ ComputeTransformOrigin(scene, style->transformOriginY, box.height) };

    return Matrix::Translate(x, y)
           * style->transform.ToMatrix(box.width, box.height)
           * Matrix::Translate(-x, -y);
}

Rect ComputeObjectFit(Scene* scene, Style* style, const Rect& box, Image* image) noexcept {
    const auto objectFitStyle{ Style::OrEmpty(style) };
    auto objectFit{ objectFitStyle->objectFit };
    float fitWidth;
    float fitHeight;
    float aspectRatio;

    if (objectFit == StyleObjectFitScaleDown) {
        if (image->WidthF() > box.width || image->HeightF() > box.height) {
            objectFit = StyleObjectFitContain;
        } else {
            objectFit = StyleObjectFitNone;
        }
    }

    switch (objectFit) {
        case StyleObjectFitContain:
            aspectRatio = image->AspectRatio();

            if (aspectRatio > (box.width / box.height)) {
                fitWidth = box.width;
                fitHeight = box.width / aspectRatio;
            } else {
                fitWidth = box.height * aspectRatio;
                fitHeight = box.height;
            }
            break;
        case StyleObjectFitCover:
            aspectRatio = image->AspectRatio();

            if (aspectRatio > (box.width / box.height)) {
                fitWidth = box.height * image->AspectRatio();
                fitHeight = box.height;
            } else {
                fitWidth = box.width;
                fitHeight = box.width / image->AspectRatio();
            }
            break;
        case StyleObjectFitNone:
            fitWidth = image->WidthF();
            fitHeight = image->HeightF();
            break;
        default:
            fitWidth = box.width;
            fitHeight = box.height;
            break;
    }

    return {
        box.x + ComputeObjectFitCoordinate(scene, objectFitStyle->objectPositionX, box.width, fitWidth),
        box.y + ComputeObjectFitCoordinate(scene, objectFitStyle->objectPositionY, box.height, fitHeight),
        fitWidth,
        fitHeight
    };
}

ImageRect ClipImage(const Rect& bounds, const Rect& imageDest, Image* image) noexcept {
    const auto ax2{ bounds.x + bounds.width };
    const auto bx2{ imageDest.x + imageDest.width };
    const auto ay2{ bounds.y + bounds.height };
    const auto by2{ imageDest.y + imageDest.height };
    const auto scaleX{ image->WidthF() / imageDest.width  };
    const auto scaleY{ image->HeightF() / imageDest.height };
    ImageRect result;

    if (imageDest.x > bounds.x) {
        result.dest.x = imageDest.x;
        result.src.x = 0;
    } else {
        result.dest.x = bounds.x;
        result.src.x = std::fabs(bounds.x - imageDest.x) * scaleX;
    }

    if (imageDest.y > bounds.y) {
        result.dest.y = imageDest.y;
        result.src.y = 0;
    } else {
        result.dest.y = bounds.y;
        result.src.y = std::fabs(bounds.y - imageDest.y) * scaleY;
    }

    result.dest.width = (bx2 < ax2) ? bx2 - result.dest.x : ax2 - result.dest.x;
    result.dest.height = (by2 < ay2) ? by2 - result.dest.y : ay2 - result.dest.y;

    result.src.width = result.dest.width * scaleX;
    result.src.height = result.dest.height * scaleY;

    // TODO: snap src rect to pixel grid?

    return result;
}

static float ComputeObjectFitCoordinate(Scene* scene, const StyleValueNumber& coordinate, float boxDimension,
        float fitDimension, float defaultPercent) noexcept {
    switch (coordinate.unit) {
        case StyleNumberUnitPoint:
            return coordinate.value;
        case StyleNumberUnitPercent:
        {
            const auto percent{ coordinate.AsPercent() };

            return (boxDimension * percent - fitDimension * percent);
        }
        case StyleNumberUnitAnchor:
            switch (coordinate.AsInt32()) {
                case StyleAnchorRight:
                case StyleAnchorBottom:
                    return boxDimension - fitDimension;
                case StyleAnchorCenter:
                    return boxDimension * .5f - fitDimension * .5f;
                default:
                    return 0;
            }
        case StyleNumberUnitViewportWidth:
            return coordinate.AsPercent() * scene->GetWidth();
        case StyleNumberUnitViewportHeight:
            return coordinate.AsPercent() * scene->GetHeight();
        case StyleNumberUnitViewportMin:
            return coordinate.AsPercent() * scene->GetViewportMin();
        case StyleNumberUnitViewportMax:
            return coordinate.AsPercent() * scene->GetViewportMax();
        case StyleNumberUnitRootEm:
            return coordinate.AsPercent() * scene->GetRootFontSize();
        default:
            return boxDimension * defaultPercent - fitDimension * defaultPercent;
    }
}

static float ComputeTransformOrigin(Scene* scene, const StyleValueNumber& transformOrigin, float dimension) noexcept {
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

} // namespace ls
