/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/StyleResolver.h>

#include <algorithm>
#include <Yoga.h>
#include <ls/Resources.h>
#include <ls/Style.h>
#include <ls/StyleValue.h>

namespace ls {

StyleResolver::StyleResolver(float width, float height, float rootFontSize) noexcept
: width(width), height(height), viewportMin(std::min(width, height)), viewportMax(std::max(width, height)),
  rootFontSize(rootFontSize) {
}

float StyleResolver::ResolveBackgroundSize(const StyleValueNumber& value, float dimension,
        float autoDimension) const noexcept {
    switch (value.unit) {
        case StyleNumberUnitPercent:
            return value.AsPercent() * dimension;
        case StyleNumberUnitPoint:
            return value.value;
        case StyleNumberUnitViewportWidth:
            return value.AsPercent() * this->width;
        case StyleNumberUnitViewportHeight:
            return value.AsPercent() * this->height;
        case StyleNumberUnitViewportMin:
            return value.AsPercent() * this->viewportMin;
        case StyleNumberUnitViewportMax:
            return value.AsPercent() * this->viewportMax;
        case StyleNumberUnitRootEm:
            return value.value * this->rootFontSize;
        case StyleNumberUnitAuto:
        case StyleNumberUnitUndefined:
            return autoDimension;
        default:
            return YGUndefined;
    }
}

float StyleResolver::ResolveTransformOrigin(const StyleValueNumber& transformOrigin,
        float dimension) const noexcept {
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
            return transformOrigin.AsPercent() * this->width;
        case StyleNumberUnitViewportHeight:
            return transformOrigin.AsPercent() * this->height;
        case StyleNumberUnitViewportMin:
            return transformOrigin.AsPercent() * this->viewportMin;
        case StyleNumberUnitViewportMax:
            return transformOrigin.AsPercent() * this->viewportMax;
        case StyleNumberUnitRootEm:
            return transformOrigin.AsPercent() * this->rootFontSize;
        case StyleNumberUnitUndefined:
            return dimension * .5f;
        default:
            return YGUnitUndefined;
    }
}

float StyleResolver::ResolveObjectFitCoordinate(const StyleValueNumber& coordinate, float dimension,
        float objectDimension, float defaultPercent) const noexcept {
    switch (coordinate.unit) {
        case StyleNumberUnitPoint:
            return coordinate.value;
        case StyleNumberUnitPercent:
        {
            const auto percent{ coordinate.AsPercent() };

            return (dimension * percent - objectDimension * percent);
        }
        case StyleNumberUnitAnchor:
            switch (coordinate.AsInt32()) {
                case StyleAnchorRight:
                case StyleAnchorBottom:
                    return dimension - objectDimension;
                case StyleAnchorCenter:
                    return dimension * .5f - objectDimension * .5f;
                default:
                    return 0;
            }
        case StyleNumberUnitViewportWidth:
            return coordinate.AsPercent() * this->width;
        case StyleNumberUnitViewportHeight:
            return coordinate.AsPercent() * this->height;
        case StyleNumberUnitViewportMin:
            return coordinate.AsPercent() * this->viewportMin;
        case StyleNumberUnitViewportMax:
            return coordinate.AsPercent() * this->viewportMax;
        case StyleNumberUnitRootEm:
            return coordinate.AsPercent() * this->rootFontSize;
        case StyleNumberUnitUndefined:
            return dimension * defaultPercent - objectDimension * defaultPercent;
        default:
            return YGUndefined;
    }
}

Matrix StyleResolver::ResolveTransform(Style* style, const Rect& box) const noexcept {
    if (!style) {
        return Matrix::Identity();
    }

    const auto x{ this->ResolveTransformOrigin(style->transformOriginX, box.width) };
    const auto y{ this->ResolveTransformOrigin(style->transformOriginY, box.height) };

    return Matrix::Translate(x, y)
        * style->transform.ToMatrix(box.width, box.height)
        * Matrix::Translate(-x, -y);
}

Rect StyleResolver::ResolveObjectFit(Style* style, const Rect& box, const Image* image) const noexcept {
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
        box.x + this->ResolveObjectFitCoordinate(objectFitStyle->objectPositionX, box.width, fitWidth, .5f),
        box.y + this->ResolveObjectFitCoordinate(objectFitStyle->objectPositionY, box.height, fitHeight, .5f),
        fitWidth,
        fitHeight
    };
}

Rect StyleResolver::ResolveBackgroundFit(Style* style, const Rect& box, const Image* image) const noexcept {
    float width;
    float height;
    auto backgroundStyle{ Style::OrEmpty(style) };

    switch (backgroundStyle->backgroundSize) {
        case StyleBackgroundSizeContain:
            if (image->AspectRatio() > (box.width / box.height)) {
                width = box.width;
                height = box.width / image->AspectRatio();
            } else {
                width = box.height * image->AspectRatio();
                height = box.height;
            }
            break;
        case StyleBackgroundSizeCover:
            if (image->AspectRatio() > (box.width / box.height)) {
                width = box.height * image->AspectRatio();
                height = box.height;
            } else {
                width = box.width;
                height = box.width / image->AspectRatio();
            }
            break;
        default:
            width = this->ResolveBackgroundSize(backgroundStyle->backgroundWidth, box.width, image->WidthF());
            height = this->ResolveBackgroundSize(backgroundStyle->backgroundHeight, box.height, image->HeightF());
            break;
    }

    return {
        box.x + this->ResolveObjectFitCoordinate(backgroundStyle->backgroundPositionX, box.width, width, 0),
        box.y + this->ResolveObjectFitCoordinate(backgroundStyle->backgroundPositionY, box.height, height, 0),
        width,
        height
    };
}

float StyleResolver::ResolveLineHeight(const StyleValueNumber& value, float fontLineHeight) const noexcept {
    switch (value.unit) {
        case StyleNumberUnitPoint:
            return value.value;
        case StyleNumberUnitPercent:
            return value.AsPercent() * fontLineHeight;
        case StyleNumberUnitViewportWidth:
            return value.AsPercent() * this->width;
        case StyleNumberUnitViewportHeight:
            return value.AsPercent() * this->height;
        case StyleNumberUnitViewportMin:
            return value.AsPercent() * this->viewportMin;
        case StyleNumberUnitViewportMax:
            return value.AsPercent() * this->viewportMax;
        case StyleNumberUnitRootEm:
            return value.value * this->rootFontSize;
        default:
            return fontLineHeight;
    }
}

float StyleResolver::ResolveFontSize(const StyleValueNumber& value) const noexcept {
    switch (value.unit) {
        case StyleNumberUnitPoint:
            return value.value;
        case StyleNumberUnitViewportWidth:
            return value.AsPercent() * this->width;
        case StyleNumberUnitViewportHeight:
            return value.AsPercent() * this->height;
        case StyleNumberUnitViewportMin:
            return value.AsPercent() * this->viewportMin;
        case StyleNumberUnitViewportMax:
            return value.AsPercent() * this->viewportMax;
        case StyleNumberUnitRootEm:
            return value.value * this->rootFontSize;
        case StyleNumberUnitUndefined:
            return 0;
        default:
            return YGUndefined;
    }
}

float StyleResolver::Update(const StyleValueNumber& value, float newWidth, float newHeight) noexcept {
    float newRootFontSize{DEFAULT_REM_FONT_SIZE};

    switch (value.unit) {
        case StyleNumberUnitPoint:
            newRootFontSize = value.value;
            break;
        case StyleNumberUnitViewportWidth:
            newRootFontSize = value.AsPercent() * newWidth;
            break;
        case StyleNumberUnitViewportHeight:
            newRootFontSize = value.AsPercent() * newHeight;
            break;
        case StyleNumberUnitViewportMin:
            newRootFontSize = value.AsPercent() * std::min(newWidth, newHeight);
            break;
        case StyleNumberUnitViewportMax:
            newRootFontSize = value.AsPercent() * std::max(newWidth, newHeight);
            break;
        case StyleNumberUnitRootEm:
            newRootFontSize = value.value * DEFAULT_REM_FONT_SIZE;
            break;
        default:
            newRootFontSize = DEFAULT_REM_FONT_SIZE;
            break;
    }

    this->width = newWidth;
    this->height = newHeight;
    this->rootFontSize = newRootFontSize;

    return newRootFontSize;
}

float StyleResolver::ResolveBorder(Style* style) const noexcept {
    return this->ResolveBorderProperty(Style::OrEmpty(style)->border, 0);
}

bool StyleResolver::HasBorderRadius(Style* style) const noexcept {
    auto borderRadius{ this->ResolveBorderRadius(Style::OrEmpty(style)) };

    return (borderRadius.topLeft > 0 || borderRadius.topRight > 0
        || borderRadius.bottomLeft > 0 || borderRadius.bottomRight);
}

BorderRadius StyleResolver::ResolveBorderRadius(Style* style) const noexcept {
    const auto p{ Style::OrEmpty(style) };
    const auto borderRadius{ this->ResolveBorderProperty(p->borderRadius, 0) };

    return {
        this->ResolveBorderProperty(p->borderRadiusTopLeft, borderRadius),
        this->ResolveBorderProperty(p->borderRadiusBottomLeft, borderRadius),
        this->ResolveBorderProperty(p->borderRadiusTopRight, borderRadius),
        this->ResolveBorderProperty(p->borderRadiusBottomRight, borderRadius)
    };
}

float StyleResolver::ResolveBorderProperty(const StyleValueNumber& value, float defaultValue) const noexcept {
    switch (value.unit) {
        case StyleNumberUnitPoint:
            return value.value;
        case StyleNumberUnitViewportWidth:
            return value.AsPercent() * this->width;
        case StyleNumberUnitViewportHeight:
            return value.AsPercent() * this->height;
        case StyleNumberUnitViewportMin:
            return value.AsPercent() * this->viewportMin;
        case StyleNumberUnitViewportMax:
            return value.AsPercent() * this->viewportMax;
        case StyleNumberUnitRootEm:
            return value.AsPercent() * this->rootFontSize;
        default:
            return defaultValue;
    }
}

int32_t StyleResolver::ResolveMaxLines(Style* style) const noexcept {
    const auto p{ Style::OrEmpty(style) };

    if (p->maxLines.unit == StyleNumberUnitPoint) {
        return p->maxLines.value;
    }

    return 0;
}

} // namespace ls
