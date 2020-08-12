/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <ls/Rect.h>
#include <ls/Matrix.h>
#include <ls/StyleValue.h>

namespace ls {

class Image;
class Style;

/**
 * Converts raw style properties into screen pixel values.
 *
 * The operations are isolated here for easier testing.
 */
class StyleResolver {
 public:
    StyleResolver() noexcept = default;
    StyleResolver(float width, float height, float rootFontSize) noexcept;

    float ResolveBackgroundSize(const StyleValueNumber& value, float dimension, float autoDimension) const noexcept;
    float ResolveTransformOrigin(const StyleValueNumber& transformOrigin, float dimension) const noexcept;
    float ResolveObjectFitCoordinate(const StyleValueNumber& coordinate, float dimension, float objectDimension,
            float defaultPercent) const noexcept;
    Matrix ResolveTransform(Style* style, const Rect& box) const noexcept;
    Rect ResolveObjectFit(Style* style, const Rect& box, const Image* image) const noexcept;
    Rect ResolveBackgroundFit(Style* style, const Rect& box, const Image* image) const noexcept;
    float ResolveLineHeight(const StyleValueNumber& value, float fontLineHeight) const noexcept;
    float ResolveFontSize(const StyleValueNumber& value) const noexcept;

 private:
    float Update(const StyleValueNumber& value, float newWidth, float newHeight) noexcept;

 private:
    float width{};
    float height{};
    float viewportMin{};
    float viewportMax{};
    float rootFontSize{};

    friend class Scene;
};

} // namespace ls
