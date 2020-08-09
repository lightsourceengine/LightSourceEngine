/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/Style.h>

#include <ls/Scene.h>
#include <ls/SceneNode.h>
#include <ls/CStringHashMap.h>
#include <napi-ext.h>

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::SafeObjectWrap;
using Napi::String;
using Napi::SymbolFor;

namespace ls {

template<
    void (*SetPoint)(YGNodeRef, YGEdge, float),
    void (*SetPercent)(YGNodeRef, YGEdge, float) = nullptr,
    void (*SetAuto)(YGNodeRef, YGEdge) = nullptr>
static void YGNodeSetNumber(YGNodeRef ygNode, StyleValueNumber& value, YGEdge edge, Scene* scene) noexcept;

template<
    void (*SetPoint)(YGNodeRef, float),
    void (*SetPercent)(YGNodeRef, float) = nullptr,
    void (*SetAuto)(YGNodeRef) = nullptr>
static void YGNodeSetNumber(YGNodeRef ygNode, StyleValueNumber& value, Scene* scene) noexcept;

Style* Style::sEmptyStyle{};
FunctionReference Style::sConstructor;

Style::Style(const CallbackInfo& info) : SafeObjectWrap<Style>(info) {
}

void Style::Init(Napi::Env env) {
    sEmptyStyle = New(env);
    sEmptyStyle->SuppressDestruct();
}

Function Style::GetClass(Napi::Env env) {
    if (sConstructor.IsEmpty()) {
        HandleScope scope(env);

        sConstructor = DefineClass(env, "Style", true, {
#define LS_ADD_PROPERTY(NAME) InstanceAccessor(#NAME, &Style::Getter_##NAME, &Style::Setter_##NAME)
            // Yoga Layout Style Properties
            LS_ADD_PROPERTY(alignItems),
            LS_ADD_PROPERTY(alignContent),
            LS_ADD_PROPERTY(alignSelf),
            LS_ADD_PROPERTY(border),
            LS_ADD_PROPERTY(borderBottom),
            LS_ADD_PROPERTY(borderLeft),
            LS_ADD_PROPERTY(borderRight),
            LS_ADD_PROPERTY(borderTop),
            LS_ADD_PROPERTY(bottom),
            LS_ADD_PROPERTY(display),
            LS_ADD_PROPERTY(flex),
            LS_ADD_PROPERTY(flexBasis),
            LS_ADD_PROPERTY(flexDirection),
            LS_ADD_PROPERTY(flexGrow),
            LS_ADD_PROPERTY(flexShrink),
            LS_ADD_PROPERTY(flexWrap),
            LS_ADD_PROPERTY(height),
            LS_ADD_PROPERTY(justifyContent),
            LS_ADD_PROPERTY(left),
            LS_ADD_PROPERTY(margin),
            LS_ADD_PROPERTY(marginBottom),
            LS_ADD_PROPERTY(marginLeft),
            LS_ADD_PROPERTY(marginRight),
            LS_ADD_PROPERTY(marginTop),
            LS_ADD_PROPERTY(maxHeight),
            LS_ADD_PROPERTY(maxWidth),
            LS_ADD_PROPERTY(minHeight),
            LS_ADD_PROPERTY(minWidth),
            LS_ADD_PROPERTY(overflow),
            LS_ADD_PROPERTY(padding),
            LS_ADD_PROPERTY(paddingBottom),
            LS_ADD_PROPERTY(paddingLeft),
            LS_ADD_PROPERTY(paddingRight),
            LS_ADD_PROPERTY(paddingTop),
            LS_ADD_PROPERTY(position),
            LS_ADD_PROPERTY(right),
            LS_ADD_PROPERTY(top),
            LS_ADD_PROPERTY(width),
            // Visual Style Properties
            LS_ADD_PROPERTY(backgroundClip),
            LS_ADD_PROPERTY(backgroundColor),
            LS_ADD_PROPERTY(backgroundHeight),
            LS_ADD_PROPERTY(backgroundImage),
            LS_ADD_PROPERTY(backgroundPositionX),
            LS_ADD_PROPERTY(backgroundPositionY),
            LS_ADD_PROPERTY(backgroundRepeat),
            LS_ADD_PROPERTY(backgroundSize),
            LS_ADD_PROPERTY(backgroundWidth),
            LS_ADD_PROPERTY(borderColor),
            LS_ADD_PROPERTY(borderRadius),
            LS_ADD_PROPERTY(borderRadiusTopLeft),
            LS_ADD_PROPERTY(borderRadiusTopRight),
            LS_ADD_PROPERTY(borderRadiusBottomLeft),
            LS_ADD_PROPERTY(borderRadiusBottomRight),
            LS_ADD_PROPERTY(color),
            LS_ADD_PROPERTY(fontFamily),
            LS_ADD_PROPERTY(fontSize),
            LS_ADD_PROPERTY(fontStyle),
            LS_ADD_PROPERTY(fontWeight),
            LS_ADD_PROPERTY(lineHeight),
            LS_ADD_PROPERTY(maxLines),
            LS_ADD_PROPERTY(objectFit),
            LS_ADD_PROPERTY(objectPositionX),
            LS_ADD_PROPERTY(objectPositionY),
            LS_ADD_PROPERTY(opacity),
            LS_ADD_PROPERTY(textAlign),
            LS_ADD_PROPERTY(textOverflow),
            LS_ADD_PROPERTY(textTransform),
            LS_ADD_PROPERTY(tintColor),
            LS_ADD_PROPERTY(transform),
            LS_ADD_PROPERTY(transformOriginX),
            LS_ADD_PROPERTY(transformOriginY),
            LS_ADD_PROPERTY(zIndex),
#undef LS_ADD_PROPERTY

            StaticValue("UnitUndefined", Number::New(env, StyleNumberUnitUndefined)),
            StaticValue("UnitPoint", Number::New(env, StyleNumberUnitPoint)),
            StaticValue("UnitPercent", Number::New(env, StyleNumberUnitPercent)),
            StaticValue("UnitViewportWidth", Number::New(env, StyleNumberUnitViewportWidth)),
            StaticValue("UnitViewportHeight", Number::New(env, StyleNumberUnitViewportHeight)),
            StaticValue("UnitViewportMin", Number::New(env, StyleNumberUnitViewportMin)),
            StaticValue("UnitViewportMax", Number::New(env, StyleNumberUnitViewportMax)),
            StaticValue("UnitAuto", Number::New(env, StyleNumberUnitAuto)),
            StaticValue("UnitAnchor", Number::New(env, StyleNumberUnitAnchor)),
            StaticValue("UnitRootEm", Number::New(env, StyleNumberUnitRootEm)),
            StaticValue("UnitRadian", Number::New(env, StyleNumberUnitRadian)),
            StaticValue("UnitDegree", Number::New(env, StyleNumberUnitDegree)),
            StaticValue("UnitGradian", Number::New(env, StyleNumberUnitGradian)),
            StaticValue("UnitTurn", Number::New(env, StyleNumberUnitTurn)),

            StaticValue("TransformIdentity", Number::New(env, StyleTransformIdentity)),
            StaticValue("TransformTranslate", Number::New(env, StyleTransformTranslate)),
            StaticValue("TransformRotate", Number::New(env, StyleTransformRotate)),
            StaticValue("TransformScale", Number::New(env, StyleTransformScale)),

            InstanceValue(SymbolFor(env, "style"), Boolean::New(env, true)),
        });
    }

    return sConstructor.Value();
}

Style* Style::New(Napi::Env env) {
    HandleScope scope(env);
    auto styleObject{ GetClass(env).New({}) };
    auto style{ Style::Cast(styleObject) };

    style->Ref();

    return style;
}

Style* Style::Empty() noexcept {
    return sEmptyStyle;
}

void Style::Bind(SceneNode* sceneNode) noexcept {
    if (this->node == sceneNode) {
        return;
    }

    if (this->node) {
        this->node->Unref();
    }

    this->node = sceneNode;

    if (this->node) {
        this->node->Ref();
    }
}

bool Style::IsLayoutOnly() const noexcept {
    return this->borderColor.empty() && this->backgroundColor.empty() && this->backgroundImage.empty();
}

bool Style::HasBorderRadius() const noexcept {
    return !this->border.empty()
        || !this->borderRadius.empty()
        || !this->borderRadiusTopLeft.empty()
        || !this->borderRadiusTopRight.empty()
        || !this->borderRadiusBottomLeft.empty()
        || !this->borderRadiusBottomRight.empty();
}

void Style::UpdateDependentProperties(bool rem, bool viewport) noexcept {
    // TODO: this is horrible, but it is functional. root font size and viewport will rarely change.
    //       style needs a better design to improve this update
    #define LS_UPDATE(PROP)                                                                 \
        switch (this->PROP.unit) {                                                          \
            case StyleNumberUnitRootEm:                                                     \
                if (rem) { this->NotifyPropertyChanged(StyleProperty::PROP); } break;       \
            case StyleNumberUnitViewportWidth:                                              \
            case StyleNumberUnitViewportHeight:                                             \
            case StyleNumberUnitViewportMin:                                                \
            case StyleNumberUnitViewportMax:                                                \
                if (viewport) { this->NotifyPropertyChanged(StyleProperty::PROP); } break;  \
            default: break;                                                                 \
        }
    #define LS_UPDATE_EDGE(PROP) \
        LS_UPDATE(PROP) \
        LS_UPDATE(PROP##Top) \
        LS_UPDATE(PROP##Right) \
        LS_UPDATE(PROP##Bottom) \
        LS_UPDATE(PROP##Left)

    LS_UPDATE_EDGE(border)
    LS_UPDATE_EDGE(margin)
    LS_UPDATE_EDGE(padding)
    LS_UPDATE(bottom)
    LS_UPDATE(flex)
    LS_UPDATE(flexBasis)
    LS_UPDATE(flexGrow)
    LS_UPDATE(flexShrink)
    LS_UPDATE(height)
    LS_UPDATE(left)
    LS_UPDATE(maxHeight)
    LS_UPDATE(maxWidth)
    LS_UPDATE(minHeight)
    LS_UPDATE(minWidth)
    LS_UPDATE(right)
    LS_UPDATE(top)
    LS_UPDATE(width)
    LS_UPDATE(backgroundHeight)
    LS_UPDATE(backgroundPositionX)
    LS_UPDATE(backgroundPositionY)
    LS_UPDATE(backgroundWidth)
    LS_UPDATE(borderRadius)
    LS_UPDATE(borderRadiusTopLeft)
    LS_UPDATE(borderRadiusTopRight)
    LS_UPDATE(borderRadiusBottomLeft)
    LS_UPDATE(borderRadiusBottomRight)
    LS_UPDATE(fontSize)
    LS_UPDATE(lineHeight)
    LS_UPDATE(maxLines)
    LS_UPDATE(objectPositionX)
    LS_UPDATE(objectPositionY)
    LS_UPDATE(opacity)
    LS_UPDATE(transformOriginX)
    LS_UPDATE(transformOriginY)
    LS_UPDATE(zIndex)

    #undef LS_UPDATE
    #undef LS_UPDATE_EDGE
}

void Style::Assign(const Style* other) noexcept {
    // The other Style object has properties that have already been validated. No constraint checks are required. The
    // Set method checks if the property has changed before assignment. If the property has changed, the bound node
    // will have it's Yoga node updated and the SceneNode will be notified of the change.

    #define LS_PROPERTY_SET(PROP) this->Set(StyleProperty::PROP, this->PROP, other->PROP)

    // Yoga Style Properties
    LS_PROPERTY_SET(alignItems);
    LS_PROPERTY_SET(alignContent);
    LS_PROPERTY_SET(alignSelf);
    LS_PROPERTY_SET(border);
    LS_PROPERTY_SET(borderBottom);
    LS_PROPERTY_SET(borderLeft);
    LS_PROPERTY_SET(borderRight);
    LS_PROPERTY_SET(borderTop);
    LS_PROPERTY_SET(bottom);
    LS_PROPERTY_SET(display);
    LS_PROPERTY_SET(flex);
    LS_PROPERTY_SET(flexBasis);
    LS_PROPERTY_SET(flexDirection);
    LS_PROPERTY_SET(flexGrow);
    LS_PROPERTY_SET(flexShrink);
    LS_PROPERTY_SET(flexWrap);
    LS_PROPERTY_SET(height);
    LS_PROPERTY_SET(justifyContent);
    LS_PROPERTY_SET(left);
    LS_PROPERTY_SET(margin);
    LS_PROPERTY_SET(marginBottom);
    LS_PROPERTY_SET(marginLeft);
    LS_PROPERTY_SET(marginRight);
    LS_PROPERTY_SET(marginTop);
    LS_PROPERTY_SET(maxHeight);
    LS_PROPERTY_SET(maxWidth);
    LS_PROPERTY_SET(minHeight);
    LS_PROPERTY_SET(minWidth);
    LS_PROPERTY_SET(overflow);
    LS_PROPERTY_SET(padding);
    LS_PROPERTY_SET(paddingBottom);
    LS_PROPERTY_SET(paddingLeft);
    LS_PROPERTY_SET(paddingRight);
    LS_PROPERTY_SET(paddingTop);
    LS_PROPERTY_SET(position);
    LS_PROPERTY_SET(right);
    LS_PROPERTY_SET(top);
    LS_PROPERTY_SET(width);
    // Visual Style Properties
    LS_PROPERTY_SET(backgroundClip);
    LS_PROPERTY_SET(backgroundColor);
    LS_PROPERTY_SET(backgroundHeight);
    LS_PROPERTY_SET(backgroundImage);
    LS_PROPERTY_SET(backgroundPositionX);
    LS_PROPERTY_SET(backgroundPositionY);
    LS_PROPERTY_SET(backgroundRepeat);
    LS_PROPERTY_SET(backgroundSize);
    LS_PROPERTY_SET(backgroundWidth);
    LS_PROPERTY_SET(borderColor);
    LS_PROPERTY_SET(borderRadius);
    LS_PROPERTY_SET(borderRadiusTopLeft);
    LS_PROPERTY_SET(borderRadiusTopRight);
    LS_PROPERTY_SET(borderRadiusBottomLeft);
    LS_PROPERTY_SET(borderRadiusBottomRight);
    LS_PROPERTY_SET(color);
    LS_PROPERTY_SET(fontFamily);
    LS_PROPERTY_SET(fontSize);
    LS_PROPERTY_SET(fontStyle);
    LS_PROPERTY_SET(fontWeight);
    LS_PROPERTY_SET(lineHeight);
    LS_PROPERTY_SET(maxLines);
    LS_PROPERTY_SET(objectFit);
    LS_PROPERTY_SET(objectPositionX);
    LS_PROPERTY_SET(objectPositionY);
    LS_PROPERTY_SET(opacity);
    LS_PROPERTY_SET(textAlign);
    LS_PROPERTY_SET(textOverflow);
    LS_PROPERTY_SET(textTransform);
    LS_PROPERTY_SET(tintColor);
    LS_PROPERTY_SET(transform);
    LS_PROPERTY_SET(transformOriginX);
    LS_PROPERTY_SET(transformOriginY);

    #undef LS_PROPERTY_SET
}

void Style::NotifyPropertyChanged(StyleProperty property) {
    if (this->node) {
        if (IsYogaLayoutProperty(property)) {
            this->SyncYogaProperty(property);
        } else {
            this->node->OnPropertyChanged(property);
        }
    }
}

void Style::SyncYogaProperty(StyleProperty property) {
    switch (property) {
        case StyleProperty::alignItems:
            YGNodeStyleSetAlignItems(this->node->ygNode, this->alignItems);
            break;
        case StyleProperty::alignContent:
            YGNodeStyleSetAlignContent(this->node->ygNode, this->alignContent);
            break;
        case StyleProperty::alignSelf:
            YGNodeStyleSetAlignSelf(this->node->ygNode, this->alignSelf);
            break;
        case StyleProperty::border:
            YGNodeSetNumber<YGNodeStyleSetBorder>(
                this->node->ygNode, this->border, YGEdgeAll, this->node->scene);
            break;
        case StyleProperty::borderBottom:
            YGNodeSetNumber<YGNodeStyleSetBorder>(
                this->node->ygNode, this->borderBottom, YGEdgeBottom, this->node->scene);
            break;
        case StyleProperty::borderLeft:
            YGNodeSetNumber<YGNodeStyleSetBorder>(
                this->node->ygNode, this->borderLeft, YGEdgeLeft, this->node->scene);
            break;
        case StyleProperty::borderRight:
            YGNodeSetNumber<YGNodeStyleSetBorder>(
                this->node->ygNode, this->borderRight, YGEdgeRight, this->node->scene);
            break;
        case StyleProperty::borderTop:
            YGNodeSetNumber<YGNodeStyleSetBorder>(
                this->node->ygNode, this->borderTop, YGEdgeTop, this->node->scene);
            break;
        case StyleProperty::bottom:
            YGNodeSetNumber<YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent>(
                this->node->ygNode, this->bottom, YGEdgeBottom, this->node->scene);
            break;
        case StyleProperty::display:
            YGNodeStyleSetDisplay(this->node->ygNode, this->display);
            break;
        case StyleProperty::flex:
            YGNodeSetNumber<YGNodeStyleSetFlex>(this->node->ygNode, this->bottom, this->node->scene);
            break;
        case StyleProperty::flexBasis:
            YGNodeSetNumber<YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto>(
                this->node->ygNode, this->flexBasis, this->node->scene);
            break;
        case StyleProperty::flexDirection:
            YGNodeStyleSetFlexDirection(this->node->ygNode, this->flexDirection);
            break;
        case StyleProperty::flexGrow:
            YGNodeSetNumber<YGNodeStyleSetFlexGrow>(this->node->ygNode, this->flexGrow, this->node->scene);
            break;
        case StyleProperty::flexShrink:
            YGNodeSetNumber<YGNodeStyleSetFlexShrink>(this->node->ygNode, this->flexShrink, this->node->scene);
            break;
        case StyleProperty::flexWrap:
            YGNodeStyleSetFlexWrap(this->node->ygNode, this->flexWrap);
            break;
        case StyleProperty::height:
            YGNodeSetNumber<YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightAuto>(
                this->node->ygNode, this->height, this->node->scene);
            break;
        case StyleProperty::justifyContent:
            YGNodeStyleSetJustifyContent(this->node->ygNode, this->justifyContent);
            break;
        case StyleProperty::left:
            YGNodeSetNumber<YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent>(
                this->node->ygNode, this->left, YGEdgeLeft, this->node->scene);
            break;
        case StyleProperty::margin:
            YGNodeSetNumber<YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>(
                this->node->ygNode, this->margin, YGEdgeAll, this->node->scene);
            break;
        case StyleProperty::marginBottom:
            YGNodeSetNumber<YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>(
                this->node->ygNode, this->marginBottom, YGEdgeBottom, this->node->scene);
            break;
        case StyleProperty::marginLeft:
            YGNodeSetNumber<YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>(
                this->node->ygNode, this->marginLeft, YGEdgeLeft, this->node->scene);
            break;
        case StyleProperty::marginRight:
            YGNodeSetNumber<YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>(
                this->node->ygNode, this->marginRight, YGEdgeRight, this->node->scene);
            break;
        case StyleProperty::marginTop:
            YGNodeSetNumber<YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto>(
                this->node->ygNode, this->marginTop, YGEdgeTop, this->node->scene);
            break;
        case StyleProperty::maxHeight:
            YGNodeSetNumber<YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent>(
                this->node->ygNode, this->maxHeight, this->node->scene);
            break;
        case StyleProperty::maxWidth:
            YGNodeSetNumber<YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent>(
                this->node->ygNode, this->maxWidth, this->node->scene);
            break;
        case StyleProperty::minHeight:
            YGNodeSetNumber<YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent>(
                this->node->ygNode, this->minHeight, this->node->scene);
            break;
        case StyleProperty::minWidth:
            YGNodeSetNumber<YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent>(
                this->node->ygNode, this->minWidth, this->node->scene);
            break;
        case StyleProperty::overflow:
            YGNodeStyleSetOverflow(this->node->ygNode, this->overflow);
            break;
        case StyleProperty::padding:
            YGNodeSetNumber<YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>(
                this->node->ygNode, this->padding, YGEdgeAll, this->node->scene);
            break;
        case StyleProperty::paddingBottom:
            YGNodeSetNumber<YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>(
                this->node->ygNode, this->paddingBottom, YGEdgeBottom, this->node->scene);
            break;
        case StyleProperty::paddingLeft:
            YGNodeSetNumber<YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>(
                this->node->ygNode, this->paddingLeft, YGEdgeLeft, this->node->scene);
            break;
        case StyleProperty::paddingRight:
            YGNodeSetNumber<YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>(
                this->node->ygNode, this->paddingRight, YGEdgeRight, this->node->scene);
            break;
        case StyleProperty::paddingTop:
            YGNodeSetNumber<YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent>(
                this->node->ygNode, this->paddingTop, YGEdgeTop, this->node->scene);
            break;
        case StyleProperty::position:
            YGNodeStyleSetPositionType(this->node->ygNode, this->position);
            break;
        case StyleProperty::right:
            YGNodeSetNumber<YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent>(
                this->node->ygNode, this->right, YGEdgeRight, this->node->scene);
            break;
        case StyleProperty::top:
            YGNodeSetNumber<YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent>(
                this->node->ygNode, this->top, YGEdgeTop, this->node->scene);
            break;
        case StyleProperty::width:
            YGNodeSetNumber<YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthAuto>(
                this->node->ygNode, this->width, this->node->scene);
            break;
        default:
            break;
    }
}

template<void (*SetPoint)(YGNodeRef, YGEdge, float), void (*SetPercent)(YGNodeRef, YGEdge, float),
    void (*SetAuto)(YGNodeRef, YGEdge)>
static void YGNodeSetNumber(YGNodeRef ygNode, StyleValueNumber& value, YGEdge edge, Scene* scene) noexcept {
    switch (value.unit) {
        case StyleNumberUnitAuto:
            if (SetAuto != nullptr) {
                SetAuto(ygNode, edge);
            }
            break;
        case StyleNumberUnitPercent:
            if (SetPercent != nullptr) {
                SetPercent(ygNode, edge, value.value);
            }
            break;
        case StyleNumberUnitPoint:
            SetPoint(ygNode, edge, value.value);
            break;
        case StyleNumberUnitViewportWidth:
            SetPoint(ygNode, edge, value.AsPercent() * scene->GetWidth());
            break;
        case StyleNumberUnitViewportHeight:
            SetPoint(ygNode, edge, value.AsPercent() * scene->GetHeight());
            break;
        case StyleNumberUnitViewportMin:
            SetPoint(ygNode, edge, value.AsPercent() * scene->GetViewportMin());
            break;
        case StyleNumberUnitViewportMax:
            SetPoint(ygNode, edge, value.AsPercent() * scene->GetViewportMax());
            break;
        case StyleNumberUnitRootEm:
            SetPoint(ygNode, edge, value.value * scene->GetRootFontSize());
            break;
        default:
            SetPoint(ygNode, edge, YGUndefined);
            break;
    }
}

template<void (*SetPoint)(YGNodeRef, float), void (*SetPercent)(YGNodeRef, float), void (*SetAuto)(YGNodeRef)>
static void YGNodeSetNumber(YGNodeRef ygNode, StyleValueNumber& value, Scene* scene) noexcept {
    switch (value.unit) {
        case StyleNumberUnitAuto:
            if (SetAuto != nullptr) {
                SetAuto(ygNode);
            }
            break;
        case StyleNumberUnitPercent:
            if (SetPercent != nullptr) {
                SetPercent(ygNode, value.value);
            }
            break;
        case StyleNumberUnitPoint:
            SetPoint(ygNode, value.value);
            break;
        case StyleNumberUnitViewportWidth:
            SetPoint(ygNode, value.AsPercent() * scene->GetWidth());
            break;
        case StyleNumberUnitViewportHeight:
            SetPoint(ygNode, value.AsPercent() * scene->GetHeight());
            break;
        case StyleNumberUnitViewportMin:
            SetPoint(ygNode, value.AsPercent() * scene->GetViewportMin());
            break;
        case StyleNumberUnitViewportMax:
            SetPoint(ygNode, value.AsPercent() * scene->GetViewportMax());
            break;
        case StyleNumberUnitRootEm:
            SetPoint(ygNode, value.value * scene->GetRootFontSize());
            break;
        default:
            SetPoint(ygNode, YGUndefined);
            break;
    }
}

} // namespace ls
