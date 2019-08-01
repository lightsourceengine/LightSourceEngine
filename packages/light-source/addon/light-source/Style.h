/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <Yoga.h>
#include <memory>
#include <map>
#include <algorithm>
#include <bitset>
#include "StyleValue.h"
#include "StyleMacros.h"

namespace ls {

class Style : public Napi::ObjectWrap<Style> {
 private:
    enum StyleFlags {
        StyleFlagsBorder,
        StyleFlagsPadding,
        StyleFlagsBorderRadius,
        StyleFlagsLayoutOnly,
    };

 public:
    explicit Style(const Napi::CallbackInfo& info);
    virtual ~Style() = default;

    // Yoga Layout Style Properties

    DefineYogaStyleEnumProperty(alignItems, YGAlign, YGNodeStyleSetAlignItems);
    DefineYogaStyleEnumProperty(alignContent, YGAlign, YGNodeStyleSetAlignContent);
    DefineYogaStyleEnumProperty(alignSelf, YGAlign, YGNodeStyleSetAlignSelf);
    DefineYogaEdgeStyleNumberProperty(border, YGEdgeAll, YGNodeStyleSetBorder);
    DefineYogaEdgeStyleNumberProperty(borderBottom, YGEdgeBottom, YGNodeStyleSetBorder);
    DefineYogaEdgeStyleNumberProperty(borderLeft, YGEdgeLeft, YGNodeStyleSetBorder);
    DefineYogaEdgeStyleNumberProperty(borderRight, YGEdgeRight, YGNodeStyleSetBorder);
    DefineYogaEdgeStyleNumberProperty(borderTop, YGEdgeTop, YGNodeStyleSetBorder);
    DefineYogaEdgeStyleNumberProperty(bottom, YGEdgeBottom, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    DefineYogaStyleEnumProperty(display, YGDisplay, YGNodeStyleSetDisplay);
    DefineYogaStyleNumberProperty(flex, YGNodeStyleSetFlex);
    DefineYogaStyleNumberProperty(
        flexBasis, YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto);
    DefineYogaStyleEnumProperty(flexDirection, YGFlexDirection, YGNodeStyleSetFlexDirection);
    DefineYogaStyleNumberProperty(flexGrow, YGNodeStyleSetFlexGrow);
    DefineYogaStyleNumberProperty(flexShrink, YGNodeStyleSetFlexShrink);
    DefineYogaStyleEnumProperty(flexWrap, YGWrap, YGNodeStyleSetFlexWrap);
    DefineYogaStyleNumberProperty(height, YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightAuto);
    DefineYogaStyleEnumProperty(justifyContent, YGJustify, YGNodeStyleSetJustifyContent);
    DefineYogaEdgeStyleNumberProperty(left, YGEdgeLeft, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    DefineYogaEdgeStyleNumberProperty(
        margin, YGEdgeAll, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    DefineYogaEdgeStyleNumberProperty(
        marginBottom, YGEdgeBottom, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    DefineYogaEdgeStyleNumberProperty(
        marginLeft, YGEdgeLeft, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    DefineYogaEdgeStyleNumberProperty(
        marginRight, YGEdgeRight, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    DefineYogaEdgeStyleNumberProperty(
        marginTop, YGEdgeTop, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto);
    DefineYogaStyleNumberProperty(maxHeight, YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent);
    DefineYogaStyleNumberProperty(maxWidth, YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent);
    DefineYogaStyleNumberProperty(minHeight, YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent);
    DefineYogaStyleNumberProperty(minWidth, YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent);
    DefineYogaStyleEnumProperty(overflow, YGOverflow, YGNodeStyleSetOverflow);
    DefineYogaEdgeStyleNumberProperty(padding, YGEdgeAll, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
    DefineYogaEdgeStyleNumberProperty(paddingBottom, YGEdgeBottom, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
    DefineYogaEdgeStyleNumberProperty(paddingLeft, YGEdgeLeft, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
    DefineYogaEdgeStyleNumberProperty(paddingRight, YGEdgeRight, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
    DefineYogaEdgeStyleNumberProperty(paddingTop, YGEdgeTop, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent);
    DefineYogaStyleEnumProperty(position, YGPositionType, YGNodeStyleSetPositionType);
    DefineYogaEdgeStyleNumberProperty(right, YGEdgeRight, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    DefineYogaEdgeStyleNumberProperty(top, YGEdgeTop, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent);
    DefineYogaStyleNumberProperty(width, YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthAuto);

    // Visual Style Properties

    DefineStyleEnumProperty(backgroundClip, StyleBackgroundClip);
    DefineStyleColorProperty(backgroundColor);
    DefineStyleNumberProperty(backgroundHeight);
    DefineStyleStringProperty(backgroundImage);
    DefineStyleEnumPropertyWithDefault(backgroundFit, StyleObjectFit, StyleObjectFitNone);
    DefineStyleNumberProperty(backgroundPositionX);
    DefineStyleNumberProperty(backgroundPositionY);
    DefineStyleNumberProperty(backgroundWidth);
    DefineStyleColorProperty(borderColor);
    DefineStyleNumberProperty(borderRadius);
    DefineStyleNumberProperty(borderRadiusTopLeft);
    DefineStyleNumberProperty(borderRadiusTopRight);
    DefineStyleNumberProperty(borderRadiusBottomLeft);
    DefineStyleNumberProperty(borderRadiusBottomRight);
    DefineStyleColorProperty(color);
    DefineStyleStringProperty(fontFamily);
    DefineStyleNumberProperty(fontSize);
    DefineStyleEnumProperty(fontStyle, StyleFontStyle);
    DefineStyleEnumProperty(fontWeight, StyleFontWeight);
    DefineStyleNumberProperty(maxLines);
    DefineStyleEnumProperty(objectFit, StyleObjectFit);
    DefineStyleNumberProperty(objectPositionX);
    DefineStyleNumberProperty(objectPositionY);
    DefineStyleNumberProperty(opacity);
    DefineStyleEnumProperty(textAlign, StyleTextAlign);
    DefineStyleEnumProperty(textOverflow, StyleTextOverflow);
    DefineStyleEnumProperty(textTransform, StyleTextTransform);
    DefineStyleColorProperty(tintColor);

    // Public Methods

    bool HasBorder() const { return this->flags[StyleFlagsBorder]; }
    bool HasPadding() const { return this->flags[StyleFlagsPadding]; }
    bool HasBorderRadius() const { return this->flags[StyleFlagsBorderRadius]; }
    bool IsLayoutOnly() const { return this->flags[StyleFlagsLayoutOnly]; }

    static Napi::Function Constructor(Napi::Env env);
    static void Init(Napi::Env env);
    static Style* Empty();

    void Apply(const YGNodeRef ygNode, const float viewportWidth, const float viewportHeight) const;
    void UpdateInternalFlags(const Napi::CallbackInfo& info);

 private:
    template<typename T>
    Napi::Value JSGetter(const std::unique_ptr<T>& styleValue, Napi::Env env) const;

    template<typename E, typename T>
    void JSSetter(std::unique_ptr<T>& styleValue, // NOLINT(runtime/references)
                  Napi::Value jsValue,
                  bool isYogaStyleValue);

 private:
    static Style* empty;
    std::vector<StyleValue *>yogaValues;
    std::bitset<4> flags;
};

} // namespace ls
