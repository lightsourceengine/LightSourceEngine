/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <lse/Matrix.h>
#include <lse/Rect.h>
#include <lse/StyleProperty.h>
#include <lse/Renderer.h>
#include <Yoga.h>

namespace lse {

class Style;
class Image;

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
  RenderFilter ComputeFilter(Style* style, color_t fallbackTint, float opacity) const noexcept;

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

 private:
  float viewportWidth;
  float viewportHeight;
  float rootFontSize;
};

} // namespace lse
