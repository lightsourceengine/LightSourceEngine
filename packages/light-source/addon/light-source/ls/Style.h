/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/StyleEnums.h>
#include <ls/StyleValue.h>
#include <Yoga.h>

namespace ls {

class SceneNode;

/**
 * Inline style declarations for a SceneNode.
 *
 * Style operates in two modes. Style can function as an immutable style "class" that can be assigned (copied) to
 * multiple SceneNodes. Style can also function as a mutable instance, exclusively owned by a SceneNode. The SceneNode
 * binds to the instance, listening for changes. In instance mode, the Style cannot be passed to or shared by another
 * SceneNode.
 *
 * A SceneNode can listen for style changes, via Bind(), and update it's state accordingly.
 *
 * Style property validation is handled by the StyleValue classes.
 *
 * The underlying structure (box model) of the scene graph is managed by a Yoga node. Some style properties map
 * directly to Yoga node style properties. In addition, there are extended properties and units that are managed
 * by a SceneNode. The Style class merges these together and distributes style property get/set appropriately.
 */
// TODO: this class uses too much memory. please fix.
class Style final : public Napi::SafeObjectWrap<Style> {
 public:
    Style(const Napi::CallbackInfo& info);
    ~Style() override = default;

    static void Init(Napi::Env env);
    static Napi::Function GetClass(Napi::Env env);
    static Style* New(Napi::Env env);
    static Style* Empty() noexcept;
    static Style* OrEmpty(Style* style) noexcept;

    void Assign(const Style* other) noexcept;
    void Bind(SceneNode* node) noexcept;
    bool IsLayoutOnly() const noexcept;
    void UpdateDependentProperties(bool rem, bool viewport) noexcept;

 public:
    #define LS_PROPERTY_BINDINGS(PROP, BOX_FUNC, UNBOX_FUNC)                                  \
        void Setter_##PROP(const Napi::CallbackInfo& info, const Napi::Value& value) {        \
            Napi::HandleScope scope(info.Env());                                              \
            this->Set_##PROP(UNBOX_FUNC(value));                                              \
        }                                                                                     \
        Napi::Value Getter_##PROP(const Napi::CallbackInfo& info) {                           \
            Napi::HandleScope scope(info.Env());                                              \
            return BOX_FUNC(info.Env(), this->PROP);                                          \
        }
    #define LS_PROPERTY(PROP, TYPE, DEFAULT, BOX_FUNC, UNBOX_FUNC)                            \
        TYPE PROP DEFAULT;                                                                    \
        void Set_##PROP(const TYPE& value) {                                                  \
            this->Set(StyleProperty::PROP, this->PROP, value);                                \
        }                                                                                     \
        LS_PROPERTY_BINDINGS(PROP, BOX_FUNC, UNBOX_FUNC)
    #define LS_PROPERTY_CONSTRAINT(PROP, TYPE, CONSTRAINT, DEFAULT, BOX_FUNC, UNBOX_FUNC)     \
        TYPE PROP DEFAULT;                                                                    \
        void Set_##PROP(const TYPE& value) {                                                  \
            this->SetWithConstraint<CONSTRAINT>(StyleProperty::PROP, this->PROP, value);      \
        }                                                                                     \
        LS_PROPERTY_BINDINGS(PROP, BOX_FUNC, UNBOX_FUNC)
    #define LS_ENUM_PROPERTY(PROP, TYPE)                                                      \
        LS_PROPERTY(PROP, TYPE, {}, StyleValueEnum::Box<TYPE>, StyleValueEnum::Unbox<TYPE>)
    #define LS_TRANSFORM_PROPERTY(PROP)                                                       \
        LS_PROPERTY(PROP, StyleValueTransform, {},                                            \
            StyleValueTransform::Box, StyleValueTransform::Unbox)
    #define LS_STRING_PROPERTY(PROP)                                                          \
        LS_PROPERTY(PROP, std::string, {}, StyleValueString::Box, StyleValueString::Unbox)
    #define LS_COLOR_PROPERTY(PROP)                                                           \
        LS_PROPERTY(PROP, StyleValueColor, {}, StyleValueColor::Box, StyleValueColor::Unbox)
    #define LS_NUMBER_PROPERTY(PROP, CONSTRAINT, DEFAULT_VALUE)                               \
        LS_PROPERTY_CONSTRAINT(PROP, StyleValueNumber, CONSTRAINT, DEFAULT_VALUE,             \
        StyleValueNumber::Box, StyleValueNumber::Unbox)
    #define LS_EDGE_PROPERTY(BASE, CONSTRAINT, DEFAULT_VALUE)                                 \
        LS_NUMBER_PROPERTY(BASE, CONSTRAINT, DEFAULT_VALUE)                                   \
        LS_NUMBER_PROPERTY(BASE##Top, CONSTRAINT, DEFAULT_VALUE)                              \
        LS_NUMBER_PROPERTY(BASE##Right, CONSTRAINT, DEFAULT_VALUE)                            \
        LS_NUMBER_PROPERTY(BASE##Bottom, CONSTRAINT, DEFAULT_VALUE)                           \
        LS_NUMBER_PROPERTY(BASE##Left, CONSTRAINT, DEFAULT_VALUE)

    LS_STRING_PROPERTY(backgroundImage)
    LS_STRING_PROPERTY(fontFamily)

    LS_COLOR_PROPERTY(backgroundColor)
    LS_COLOR_PROPERTY(borderColor)
    LS_COLOR_PROPERTY(color)
    LS_COLOR_PROPERTY(tintColor)

    LS_EDGE_PROPERTY(border, PointOnlyGTEZeroConstraint, {})
    LS_EDGE_PROPERTY(margin, MarginConstraint, {})
    LS_EDGE_PROPERTY(padding, PointPercentOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(bottom, PointPercentOnlyConstraint, {})
    LS_NUMBER_PROPERTY(flex, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(flexBasis, GTEZeroConstraint, {StyleNumberUnitAuto})
    LS_NUMBER_PROPERTY(flexGrow, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(flexShrink, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(height, GTEZeroConstraint, {StyleNumberUnitAuto})
    LS_NUMBER_PROPERTY(left, PointPercentOnlyConstraint, {})
    LS_NUMBER_PROPERTY(maxHeight, PointPercentOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(maxWidth, PointPercentOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(minHeight, PointPercentOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(minWidth, PointPercentOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(right, PointPercentOnlyConstraint, {})
    LS_NUMBER_PROPERTY(top, PointPercentOnlyConstraint, {})
    LS_NUMBER_PROPERTY(width, GTEZeroConstraint, {StyleNumberUnitAuto})

    LS_NUMBER_PROPERTY(backgroundHeight, PointPercentOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(backgroundPositionX, ObjectPositionConstraint, {})
    LS_NUMBER_PROPERTY(backgroundPositionY, ObjectPositionConstraint, {})
    LS_NUMBER_PROPERTY(backgroundWidth, PointPercentOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(borderRadius, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(borderRadiusTopLeft, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(borderRadiusTopRight, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(borderRadiusBottomLeft, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(borderRadiusBottomRight, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(fontSize, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(lineHeight, PointPercentOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(maxLines, PointOnlyGTEZeroConstraint, {})
    LS_NUMBER_PROPERTY(objectPositionX, ObjectPositionConstraint, {})
    LS_NUMBER_PROPERTY(objectPositionY, ObjectPositionConstraint, {})
    LS_NUMBER_PROPERTY(opacity, OpacityConstraint, {})
    LS_NUMBER_PROPERTY(transformOriginX, ObjectPositionConstraint, {})
    LS_NUMBER_PROPERTY(transformOriginY, ObjectPositionConstraint, {})
    LS_NUMBER_PROPERTY(zIndex, ZIndexConstraint, {})

    LS_TRANSFORM_PROPERTY(transform)

    LS_ENUM_PROPERTY(alignContent, YGAlign)
    LS_ENUM_PROPERTY(alignItems, YGAlign)
    LS_ENUM_PROPERTY(alignSelf, YGAlign)
    LS_ENUM_PROPERTY(display, YGDisplay)
    LS_ENUM_PROPERTY(flexDirection, YGFlexDirection)
    LS_ENUM_PROPERTY(flexWrap, YGWrap)
    LS_ENUM_PROPERTY(justifyContent, YGJustify)
    LS_ENUM_PROPERTY(overflow, YGOverflow)
    LS_ENUM_PROPERTY(position, YGPositionType)

    LS_ENUM_PROPERTY(backgroundClip, StyleBackgroundClip)
    LS_ENUM_PROPERTY(backgroundRepeat, StyleBackgroundRepeat)
    LS_ENUM_PROPERTY(backgroundSize, StyleBackgroundSize)
    LS_ENUM_PROPERTY(fontStyle, StyleFontStyle)
    LS_ENUM_PROPERTY(fontWeight, StyleFontWeight)
    LS_ENUM_PROPERTY(objectFit, StyleObjectFit)
    LS_ENUM_PROPERTY(textAlign, StyleTextAlign)
    LS_ENUM_PROPERTY(textOverflow, StyleTextOverflow)
    LS_ENUM_PROPERTY(textTransform, StyleTextTransform)

    #undef LS_EDGE_PROPERTY
    #undef LS_NUMBER_PROPERTY
    #undef LS_COLOR_PROPERTY
    #undef LS_STRING_PROPERTY
    #undef LS_ENUM_PROPERTY
    #undef LS_PROPERTY
    #undef LS_PROPERTY_CONSTRAINT
    #undef LS_PROPERTY_BINDINGS

 private:
    void NotifyPropertyChanged(StyleProperty property);
    void SyncYogaProperty(StyleProperty property) noexcept;

    template<typename T>
    void Set(StyleProperty name, T& property, const T& value);

    template<typename T>
    void Set(StyleProperty name, T& property, T&& value);

    template<typename Constraint>
    void SetWithConstraint(StyleProperty name, StyleValueNumber& property, const StyleValueNumber& value);

    void SetYogaValue(StyleValueNumber& value, void (*SetPoint)(YGNodeRef, float),
            void (*SetPercent)(YGNodeRef, float) = nullptr, void (*SetAuto)(YGNodeRef) = nullptr) noexcept;

    void SetYogaValue(StyleValueNumber& value, YGEdge edge,
                      void (*SetPoint)(YGNodeRef, YGEdge, float),
                      void (*SetPercent)(YGNodeRef, YGEdge, float) = nullptr,
                      void (*SetAuto)(YGNodeRef, YGEdge) = nullptr) noexcept;

 private:
    static Style* sEmptyStyle;
    static Napi::FunctionReference sConstructor;

    SceneNode* node{};
};

template<typename T>
void Style::Set(StyleProperty name, T& property, const T& value) {
    if (!(property == value)) {
        property = value;
        this->NotifyPropertyChanged(name);
    }
}

template<typename T>
void Style::Set(StyleProperty name, T& property, T&& value) {
    if (!(property == value)) {
        property = std::forward<T>(value);
        this->NotifyPropertyChanged(name);
    }
}

template<typename Constraint>
void Style::SetWithConstraint(StyleProperty name, StyleValueNumber& property, const StyleValueNumber& value) {
    this->Set(name, property, (value.empty() || !Constraint()(value)) ? StyleValueNumber::OfUndefined() : value);
}

} // namespace ls
