/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/StyleContext.h>

#include <algorithm>
#include <std17/algorithm>
#include <std20/numbers>
#include <Yoga.h>
#include <YGStyle.h>
#include <lse/Style.h>
#include <lse/Resources.h>

namespace lse {

static YGStyle kYogaStyleDefaults{};

static Matrix ToMatrix(const std::vector<StyleTransformSpec>& transform, const Rect& box) noexcept;

StyleContext::StyleContext(float viewportWidth, float viewportHeight, float rootFontSize) noexcept
    : viewportWidth(viewportWidth), viewportHeight(viewportHeight), rootFontSize(rootFontSize) {
}

float StyleContext::ComputeViewportWidthUnit(float value) const noexcept {
  return (value / 100.f) * this->viewportWidth;
}

float StyleContext::ComputeViewportHeightUnit(float value) const noexcept {
  return (value / 100.f) * this->viewportHeight;
}

float StyleContext::ComputeViewportMinUnit(float value) const noexcept {
  return (value / 100.f) * std::min(this->viewportWidth, this->viewportHeight);
}

float StyleContext::ComputeViewportMaxUnit(float value) const noexcept {
  return (value / 100.f) * std::max(this->viewportWidth, this->viewportHeight);
}

float StyleContext::ComputeRemUnit(float value) const noexcept {
  return (value / 100.f) * this->rootFontSize;
}

float StyleContext::ComputeFontSize(Style* style, bool overrideRem) const noexcept {
  assert(style);
  const auto& styleValue{ style->GetNumber(StyleProperty::fontSize) };

  switch (styleValue.unit) {
    case StyleNumberUnitPoint:
      return styleValue.value;
    case StyleNumberUnitViewportWidth:
      return this->ComputeViewportWidthUnit(styleValue.value);
    case StyleNumberUnitViewportHeight:
      return this->ComputeViewportHeightUnit(styleValue.value);
    case StyleNumberUnitViewportMin:
      return this->ComputeViewportMinUnit(styleValue.value);
    case StyleNumberUnitViewportMax:
      return this->ComputeViewportMaxUnit(styleValue.value);
    case StyleNumberUnitRootEm:
      return overrideRem ? DEFAULT_REM_FONT_SIZE : this->ComputeRemUnit(styleValue.value);
    default:
      return DEFAULT_REM_FONT_SIZE;
  }
}

float StyleContext::ComputeOpacity(Style* style) const noexcept {
  assert(style);
  const auto& styleValue = style->GetNumber(StyleProperty::opacity);

  switch (styleValue.unit) {
    case StyleNumberUnitPoint:
      return std17::clamp(styleValue.value, 0.f, 1.f);
    case StyleNumberUnitPercent:
      return std17::clamp(styleValue.value / 100.f, 0.f, 1.f);
    default:
      return 1.f;
  }
}

Matrix StyleContext::ComputeTransform(Style* style, const Rect& box) const noexcept {
  assert(style);

  if (style->IsEmpty(StyleProperty::transform)) {
    return Matrix::Identity();
  }

  const auto x{ this->ComputeTransformOrigin(style, StyleProperty::transformOriginX, box.width) };
  const auto y{ this->ComputeTransformOrigin(style, StyleProperty::transformOriginY, box.height) };

  return Matrix::Translate(x, y)
      * ToMatrix(style->GetTransform(), box)
      * Matrix::Translate(-x, -y);
}

float StyleContext::ComputeTransformOrigin(Style* style, StyleProperty property, float dimension) const noexcept {
  const auto& styleValue = style->GetNumber(property);

  switch (styleValue.unit) {
    case StyleNumberUnitPoint:
      return styleValue.value;
    case StyleNumberUnitPercent:
      return (styleValue.value / 100.f) * dimension;
    case StyleNumberUnitAnchor: {
      switch (static_cast<int32_t>(styleValue.value)) {
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
      return this->ComputeViewportWidthUnit(styleValue.value);
    case StyleNumberUnitViewportHeight:
      return this->ComputeViewportHeightUnit(styleValue.value);
    case StyleNumberUnitViewportMin:
      return this->ComputeViewportMinUnit(styleValue.value);
    case StyleNumberUnitViewportMax:
      return this->ComputeViewportMaxUnit(styleValue.value);
    case StyleNumberUnitRootEm:
      return this->ComputeRemUnit(styleValue.value);
    default:
      return dimension * .5f;
  }
}

bool StyleContext::HasBorderRadius(Style* style) const noexcept {
  assert(style);
  return style->Exists(StyleProperty::borderRadius)
      || style->Exists(StyleProperty::borderRadiusBottomRight)
      || style->Exists(StyleProperty::borderRadiusBottomLeft)
      || style->Exists(StyleProperty::borderRadiusTopRight)
      || style->Exists(StyleProperty::borderRadiusTopLeft);
}

Rect StyleContext::ComputeObjectFit(Style* style, const Rect& box, const Image* image) const noexcept {
  assert(style);
  auto objectFit{ style->GetEnum<StyleObjectFit>(StyleProperty::objectFit) };
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
      box.x + this->ComputeObjectFitCoordinate(style, StyleProperty::objectPositionX, box.width, fitWidth, .5f),
      box.y + this->ComputeObjectFitCoordinate(style, StyleProperty::objectPositionY, box.height, fitHeight, .5f),
      fitWidth,
      fitHeight
  };
}

float StyleContext::ComputeBackgroundSize(
    Style* style, StyleProperty property,
    float dimension, float autoDimension) const noexcept {
  assert(style);
  const auto& styleValue = style->GetNumber(property);

  switch (styleValue.unit) {
    case StyleNumberUnitPercent:
      return (styleValue.value / 100.f) * dimension;
    case StyleNumberUnitPoint:
      return styleValue.value;
    case StyleNumberUnitViewportWidth:
      return this->ComputeViewportWidthUnit(styleValue.value);
    case StyleNumberUnitViewportHeight:
      return this->ComputeViewportHeightUnit(styleValue.value);
    case StyleNumberUnitViewportMin:
      return this->ComputeViewportMinUnit(styleValue.value);
    case StyleNumberUnitViewportMax:
      return this->ComputeViewportMaxUnit(styleValue.value);
    case StyleNumberUnitRootEm:
      return this->ComputeRemUnit(styleValue.value);
    case StyleNumberUnitAuto:
    case StyleNumberUnitUndefined:
      return autoDimension;
    default:
      return kUndefined;
  }
}

float StyleContext::ComputeObjectFitCoordinate(
    Style* style, StyleProperty property, float dimension,
    float objectDimension, float defaultPercent) const noexcept {
  assert(style);
  const auto& styleValue = style->GetNumber(property);

  switch (styleValue.unit) {
    case StyleNumberUnitPoint:
      return styleValue.value;
    case StyleNumberUnitPercent: {
      const auto percent{ styleValue.value / 100.f };

      return (dimension * percent - objectDimension * percent);
    }
    case StyleNumberUnitAnchor:
      switch (static_cast<int32_t>(styleValue.value)) {
        case StyleAnchorRight:
        case StyleAnchorBottom:
          return dimension - objectDimension;
        case StyleAnchorCenter:
          return dimension * .5f - objectDimension * .5f;
        default:
          return 0;
      }
    case StyleNumberUnitViewportWidth:
      return this->ComputeViewportWidthUnit(styleValue.value);
    case StyleNumberUnitViewportHeight:
      return this->ComputeViewportHeightUnit(styleValue.value);
    case StyleNumberUnitViewportMin:
      return this->ComputeViewportMinUnit(styleValue.value);
    case StyleNumberUnitViewportMax:
      return this->ComputeViewportMaxUnit(styleValue.value);
    case StyleNumberUnitRootEm:
      return this->ComputeRemUnit(styleValue.value);
    case StyleNumberUnitUndefined:
      return dimension * defaultPercent - objectDimension * defaultPercent;
    default:
      return kUndefined;
  }
}

Rect StyleContext::ComputeBackgroundFit(Style* style, const Rect& box, const Image* image) const noexcept {
  assert(style);
  float width;
  float height;

  switch (style->GetEnum(StyleProperty::backgroundSize)) {
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
      width = this->ComputeBackgroundSize(style, StyleProperty::backgroundWidth, box.width, image->WidthF());
      height = this->ComputeBackgroundSize(style, StyleProperty::backgroundHeight, box.height, image->HeightF());
      break;
  }

  return {
      box.x + this->ComputeObjectFitCoordinate(style, StyleProperty::backgroundPositionX, box.width, width, 0),
      box.y + this->ComputeObjectFitCoordinate(style, StyleProperty::backgroundPositionY, box.height, height, 0),
      width,
      height
  };
}

float StyleContext::ComputeLineHeight(Style* style, float fontLineHeight) const noexcept {
  assert(style);
  const auto& styleValue = style->GetNumber(StyleProperty::lineHeight);

  switch (styleValue.unit) {
    case StyleNumberUnitPoint:
      return styleValue.value * fontLineHeight;
    case StyleNumberUnitPercent:
      return (styleValue.value / 100.f) * fontLineHeight;
    case StyleNumberUnitViewportWidth:
      return this->ComputeViewportWidthUnit(styleValue.value);
    case StyleNumberUnitViewportHeight:
      return this->ComputeViewportHeightUnit(styleValue.value);
    case StyleNumberUnitViewportMin:
      return this->ComputeViewportMinUnit(styleValue.value);
    case StyleNumberUnitViewportMax:
      return this->ComputeViewportMaxUnit(styleValue.value);
    case StyleNumberUnitRootEm:
      return this->ComputeRemUnit(styleValue.value);
    default:
      return fontLineHeight;
  }
}

BorderRadiusCorners StyleContext::ComputeBorderRadius(Style* style, const Rect& box) const noexcept {
  assert(style);
  const auto radiusLimit{ std::min(box.width, box.height) };
  const auto borderRadius{ this->ComputeBorderRadiusProperty(style, StyleProperty::borderRadius, radiusLimit, 0) };

  return {
      this->ComputeBorderRadiusProperty(style, StyleProperty::borderRadiusTopLeft, radiusLimit, borderRadius),
      this->ComputeBorderRadiusProperty(style, StyleProperty::borderRadiusBottomLeft, radiusLimit, borderRadius),
      this->ComputeBorderRadiusProperty(style, StyleProperty::borderRadiusTopRight, radiusLimit, borderRadius),
      this->ComputeBorderRadiusProperty(style, StyleProperty::borderRadiusBottomRight, radiusLimit, borderRadius)
  };
}

float StyleContext::ComputeBorderRadiusProperty(
    Style* style, StyleProperty property, float dimension,
    float defaultValue) const noexcept {
  float result;
  const auto& styleValue = style->GetNumber(property);

  switch (styleValue.unit) {
    case StyleNumberUnitPoint:
      result = styleValue.value;
      break;
    case StyleNumberUnitPercent:
      result = (styleValue.value / 100.f) * dimension;
      break;
    case StyleNumberUnitViewportWidth:
      result = this->ComputeViewportWidthUnit(styleValue.value);
      break;
    case StyleNumberUnitViewportHeight:
      result = this->ComputeViewportHeightUnit(styleValue.value);
      break;
    case StyleNumberUnitViewportMin:
      result = this->ComputeViewportMinUnit(styleValue.value);
      break;
    case StyleNumberUnitViewportMax:
      result = this->ComputeViewportMaxUnit(styleValue.value);
      break;
    case StyleNumberUnitRootEm:
      result = this->ComputeRemUnit(styleValue.value);
      break;
    default:
      result = defaultValue;
      break;
  }

  return std::min(result, dimension * .5f);
}

void StyleContext::SetViewportSize(float width, float height) noexcept {
  this->viewportWidth = width;
  this->viewportHeight = height;
}

void StyleContext::SetRootFontSize(float fontSize) noexcept {
  this->rootFontSize = fontSize;
}

constexpr float ComputeRotateAngle(const StyleValue& styleValue) noexcept {
  switch (styleValue.unit) {
    case StyleNumberUnitDegree:
      return styleValue.value * (std20::pi_v<float> / 180.f);
    case StyleNumberUnitGradian:
      return styleValue.value * (std20::pi_v<float> / 200.f);
    case StyleNumberUnitTurn:
      return styleValue.value * 2 * std20::pi_v<float>;
    default:
      return styleValue.value; // radians
  }
}

constexpr float ComputeTranslateAxis(const StyleValue& styleValue, float containerLength) noexcept {
  switch (styleValue.unit) {
    case StyleNumberUnitPercent:
      return (styleValue.value / 100.f) * containerLength;
    default:
      return styleValue.value;
  }
}

static Matrix ToMatrix(const std::vector<StyleTransformSpec>& transform, const Rect& box) noexcept {
  auto result{ Matrix::Identity() };

  for (const auto& t : transform) {
    switch (t.transform) {
      case StyleTransformIdentity:
        result *= Matrix::Identity();
        break;
      case StyleTransformTranslate:
        result *= Matrix::Translate(
            ComputeTranslateAxis(t.x, box.width), ComputeTranslateAxis(t.y, box.height));
        break;
      case StyleTransformScale:
        result *= Matrix::Scale(t.x.value, t.y.value);
        break;
      case StyleTransformRotate:
        result *= Matrix::Rotate(ComputeRotateAngle(t.angle));
        break;
      default:
        assert(false);
        break;
    }
  }

  return result;
}

using EdgePointFunc = decltype(&YGNodeStyleSetMargin);
using EdgePercentFunc = decltype(&YGNodeStyleSetMarginPercent);
using EdgeAutoFunc = decltype(&YGNodeStyleSetMarginAuto);

static void EmptyEdgeSetPercent(YGNodeRef node, YGEdge edge, float value) noexcept {}
static void EmptyEdgeSetAuto(YGNodeRef node, YGEdge edge) noexcept {}

template<
    YGEdge tEdge,
    EdgePointFunc tSetPoint,
    EdgePercentFunc tSetPercent = EmptyEdgeSetPercent,
    EdgeAutoFunc tSetAuto = EmptyEdgeSetAuto>
struct YGApplyEdge {
  static void SetPoint(YGNodeRef node, float value) noexcept {
    tSetPoint(node, tEdge, value);
  }

  static void SetPercent(YGNodeRef node, float value) noexcept {
    tSetPercent(node, tEdge, value);
  }

  static void SetAuto(YGNodeRef node) noexcept {
    tSetAuto(node, tEdge);
  }
};

using PointFunc = decltype(&YGNodeStyleSetWidth);
using PercentFunc = decltype(&YGNodeStyleSetWidthPercent);
using AutoFunc = decltype(&YGNodeStyleSetWidthAuto);

static void EmptySetPercent(YGNodeRef node, float value) noexcept {}
static void EmptySetAuto(YGNodeRef node) noexcept {}

template<
    PointFunc tSetPoint,
    PercentFunc tSetPercent = EmptySetPercent,
    AutoFunc tSetAuto = EmptySetAuto>
struct YGApply {
  static void SetPoint(YGNodeRef node, float value) noexcept {
    tSetPoint(node, value);
  }

  static void SetPercent(YGNodeRef node, float value) noexcept {
    tSetPercent(node, value);
  }

  static void SetAuto(YGNodeRef node) noexcept {
    tSetAuto(node);
  }
};

template<typename T>
static void Set(YGNodeRef node, const StyleValue& styleValue, const StyleContext* context) noexcept {
  switch (styleValue.unit) {
    case StyleNumberUnitAuto:
      T::SetAuto(node);
      break;
    case StyleNumberUnitPercent:
      T::SetPercent(node, styleValue.value);
      break;
    case StyleNumberUnitPoint:
      T::SetPoint(node, styleValue.value);
      break;
    case StyleNumberUnitViewportWidth:
      T::SetPoint(node, context->ComputeViewportWidthUnit(styleValue.value));
      break;
    case StyleNumberUnitViewportHeight:
      T::SetPoint(node, context->ComputeViewportHeightUnit(styleValue.value));
      break;
    case StyleNumberUnitViewportMin:
      T::SetPoint(node, context->ComputeViewportMinUnit(styleValue.value));
      break;
    case StyleNumberUnitViewportMax:
      T::SetPoint(node, context->ComputeViewportMaxUnit(styleValue.value));
      break;
    case StyleNumberUnitRootEm:
      T::SetPoint(node, context->ComputeRemUnit(styleValue.value));
      break;
    default:
      T::SetPoint(node, kUndefined);
      break;
  }
}

void StyleContext::SetYogaPropertyValue(Style* style, StyleProperty property, YGNodeRef target) const noexcept {
  const auto isDefined{ !style->IsEmpty(property) };

#define LS_ENUM_OR(PROP, ENUM) isDefined ? style->GetEnum<ENUM>(StyleProperty::PROP) : kYogaStyleDefaults.PROP()

  switch (property) {
    case StyleProperty::alignItems:
      YGNodeStyleSetAlignItems(target, LS_ENUM_OR(alignItems, YGAlign));
      break;
    case StyleProperty::alignContent:
      YGNodeStyleSetAlignContent(target, LS_ENUM_OR(alignContent, YGAlign));
      break;
    case StyleProperty::alignSelf:
      YGNodeStyleSetAlignSelf(target, LS_ENUM_OR(alignSelf, YGAlign));
      break;
    case StyleProperty::border:
      Set<YGApplyEdge<YGEdgeAll, YGNodeStyleSetBorder>>(target, style->GetNumber(property), this);
      break;
    case StyleProperty::borderBottom:
      Set<YGApplyEdge<YGEdgeBottom, YGNodeStyleSetBorder>>(target, style->GetNumber(property), this);
      break;
    case StyleProperty::borderLeft:
      Set<YGApplyEdge<YGEdgeLeft, YGNodeStyleSetBorder>>(target, style->GetNumber(property), this);
      break;
    case StyleProperty::borderRight:
      Set<YGApplyEdge<YGEdgeRight, YGNodeStyleSetBorder>>(target, style->GetNumber(property), this);
      break;
    case StyleProperty::borderTop:
      Set<YGApplyEdge<YGEdgeTop, YGNodeStyleSetBorder>>(target, style->GetNumber(property), this);
      break;
    case StyleProperty::bottom:
      Set<YGApplyEdge<YGEdgeBottom, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::display:
      YGNodeStyleSetDisplay(target, LS_ENUM_OR(display, YGDisplay));
      break;
    case StyleProperty::flex:
      Set<YGApply<YGNodeStyleSetFlex>>(target, style->GetNumber(property), this);
      break;
    case StyleProperty::flexBasis:
      Set<YGApply<YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto>>(
          target, isDefined ? style->GetNumber(property) : StyleValue::OfAuto(), this);
      break;
    case StyleProperty::flexDirection:
      YGNodeStyleSetFlexDirection(target, LS_ENUM_OR(flexDirection, YGFlexDirection));
      break;
    case StyleProperty::flexGrow:
      Set<YGApply<YGNodeStyleSetFlexGrow>>(target, style->GetNumber(property), this);
      break;
    case StyleProperty::flexShrink:
      Set<YGApply<YGNodeStyleSetFlexShrink>>(target, style->GetNumber(property), this);
      break;
    case StyleProperty::flexWrap:
      YGNodeStyleSetFlexWrap(target, LS_ENUM_OR(flexWrap, YGWrap));
      break;
    case StyleProperty::height:
      Set<YGApply<YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightAuto>>(
          target, isDefined ? style->GetNumber(property) : StyleValue::OfAuto(), this);
      break;
    case StyleProperty::justifyContent:
      YGNodeStyleSetJustifyContent(target, LS_ENUM_OR(justifyContent, YGJustify));
      break;
    case StyleProperty::left:
      Set<YGApplyEdge<YGEdgeLeft, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::margin:
      Set<YGApplyEdge<YGEdgeAll, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::marginBottom:
      Set<YGApplyEdge<YGEdgeBottom, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::marginLeft:
      Set<YGApplyEdge<YGEdgeLeft, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::marginRight:
      Set<YGApplyEdge<YGEdgeRight, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::marginTop:
      Set<YGApplyEdge<YGEdgeTop, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::maxHeight:
      Set<YGApply<YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::maxWidth:
      Set<YGApply<YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::minHeight:
      Set<YGApply<YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::minWidth:
      Set<YGApply<YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::overflow:
      YGNodeStyleSetOverflow(target, LS_ENUM_OR(overflow, YGOverflow));
      break;
    case StyleProperty::padding:
      Set<YGApplyEdge<YGEdgeAll, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::paddingBottom:
      Set<YGApplyEdge<YGEdgeBottom, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::paddingLeft:
      Set<YGApplyEdge<YGEdgeLeft, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::paddingRight:
      Set<YGApplyEdge<YGEdgeRight, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::paddingTop:
      Set<YGApplyEdge<YGEdgeTop, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::position:
      YGNodeStyleSetPositionType(target,
                                 isDefined ? style->GetEnum<YGPositionType>(property)
                                           : kYogaStyleDefaults.positionType());
      break;
    case StyleProperty::right:
      Set<YGApplyEdge<YGEdgeRight, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::top:
      Set<YGApplyEdge<YGEdgeTop, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent>>(
          target, style->GetNumber(property), this);
      break;
    case StyleProperty::width:
      Set<YGApply<YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthAuto>>(
          target, isDefined ? style->GetNumber(property) : StyleValue::OfAuto(), this);
      break;
    default:
      break;
  }

#undef LS_ENUM_OR
}

} // namespace lse
