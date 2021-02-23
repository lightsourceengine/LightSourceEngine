/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <lse/Matrix.h>
#include <lse/Rect.h>
#include <lse/StyleProperty.h>
#include <Yoga.h>

namespace lse {

class Style;
class Image;

/**
 * Border radius corners in screen pixel space.
 */
struct BorderRadiusCorners {
  float topLeft;
  float bottomLeft;
  float topRight;
  float bottomRight;
};

/**
 * Style processing API for SceneNodes.
 */
class StyleContext {
 public:
  StyleContext(float viewportWidth, float viewportHeight, float rootFontSize) noexcept;

  // Compute custom units: rem, vw, vh, vmin, vmax
  float ComputeViewportWidthUnit(float value) const noexcept;
  float ComputeViewportHeightUnit(float value) const noexcept;
  float ComputeViewportMinUnit(float value) const noexcept;
  float ComputeViewportMaxUnit(float value) const noexcept;
  float ComputeRemUnit(float value) const noexcept;

  // Compute specific properties
  float ComputeFontSize(Style* style, bool overrideRem = false) const noexcept;
  float ComputeOpacity(Style* style) const noexcept;
  Matrix ComputeTransform(Style* style, const Rect& box) const noexcept;
  Rect ComputeObjectFit(Style* style, const Rect& box, const Image* image) const noexcept;
  Rect ComputeBackgroundFit(Style* style, const Rect& box, const Image* image) const noexcept;
  float ComputeLineHeight(Style* style, float fontLineHeight) const noexcept;
  BorderRadiusCorners ComputeBorderRadius(Style* style, const Rect& box) const noexcept;

  // Query style state
  bool HasBorderRadius(Style* style) const noexcept;

  // Set environment context variables: viewport width, viewport height, root font size
  void SetViewportSize(float width, float height) noexcept;
  void SetRootFontSize(float fontSize) noexcept;

  // Yoga property binding
  void SetYogaPropertyValue(Style* style, StyleProperty property, YGNodeRef target) const noexcept;

  float ComputeTransformOrigin(Style* style, StyleProperty property, float dimension) const noexcept;

 private:
  float ComputeBackgroundSize(
      Style* style, StyleProperty property,
      float dimension, float autoDimension) const noexcept;
  float ComputeObjectFitCoordinate(
      Style* style, StyleProperty property,
      float dimension, float objectDimension, float defaultPercent) const noexcept;
  float ComputeBorderRadiusProperty(
      Style* style, StyleProperty property,
      float dimension, float defaultValue) const noexcept;

 private:
  float viewportWidth;
  float viewportHeight;
  float rootFontSize;
};

} // namespace lse
