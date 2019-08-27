/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Style.h"
#include <napi-ext.h>
#include <YGNode.h>
#include <YGStyle.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Value;

namespace ls {

static YGStyle sEmptyStyle{};
Style* Style::empty{};

void EmptySetAuto(const YGNodeRef node) {}
void EmptySetAuto(const YGNodeRef node, const YGEdge) {}
void EmptySetPercent(const YGNodeRef node, const float) {}
void EmptySetPercent(const YGNodeRef node, const YGEdge, const float) {}

Style::Style(const CallbackInfo& info) : ObjectWrap<Style>(info) {
}

Function Style::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "StyleBase", {
            // Yoga Layout Style Properties

            StylePropertyInstanceAccessor(alignItems),
            StylePropertyInstanceAccessor(alignContent),
            StylePropertyInstanceAccessor(alignSelf),
            StylePropertyInstanceAccessor(border),
            StylePropertyInstanceAccessor(borderBottom),
            StylePropertyInstanceAccessor(borderLeft),
            StylePropertyInstanceAccessor(borderRight),
            StylePropertyInstanceAccessor(borderTop),
            StylePropertyInstanceAccessor(bottom),
            StylePropertyInstanceAccessor(display),
            StylePropertyInstanceAccessor(flex),
            StylePropertyInstanceAccessor(flexBasis),
            StylePropertyInstanceAccessor(flexDirection),
            StylePropertyInstanceAccessor(flexGrow),
            StylePropertyInstanceAccessor(flexShrink),
            StylePropertyInstanceAccessor(flexWrap),
            StylePropertyInstanceAccessor(height),
            StylePropertyInstanceAccessor(justifyContent),
            StylePropertyInstanceAccessor(left),
            StylePropertyInstanceAccessor(margin),
            StylePropertyInstanceAccessor(marginBottom),
            StylePropertyInstanceAccessor(marginLeft),
            StylePropertyInstanceAccessor(marginRight),
            StylePropertyInstanceAccessor(marginTop),
            StylePropertyInstanceAccessor(maxHeight),
            StylePropertyInstanceAccessor(maxWidth),
            StylePropertyInstanceAccessor(minHeight),
            StylePropertyInstanceAccessor(minWidth),
            StylePropertyInstanceAccessor(overflow),
            StylePropertyInstanceAccessor(padding),
            StylePropertyInstanceAccessor(paddingBottom),
            StylePropertyInstanceAccessor(paddingLeft),
            StylePropertyInstanceAccessor(paddingRight),
            StylePropertyInstanceAccessor(paddingTop),
            StylePropertyInstanceAccessor(position),
            StylePropertyInstanceAccessor(right),
            StylePropertyInstanceAccessor(top),
            StylePropertyInstanceAccessor(width),

            // Visual Style Properties

            StylePropertyInstanceAccessor(backgroundClip),
            StylePropertyInstanceAccessor(backgroundColor),
            StylePropertyInstanceAccessor(backgroundHeight),
            StylePropertyInstanceAccessor(backgroundImage),
            StylePropertyInstanceAccessor(backgroundFit),
            StylePropertyInstanceAccessor(backgroundPositionX),
            StylePropertyInstanceAccessor(backgroundPositionY),
            StylePropertyInstanceAccessor(backgroundWidth),
            StylePropertyInstanceAccessor(borderColor),
            StylePropertyInstanceAccessor(borderRadius),
            StylePropertyInstanceAccessor(borderRadiusTopLeft),
            StylePropertyInstanceAccessor(borderRadiusTopRight),
            StylePropertyInstanceAccessor(borderRadiusBottomLeft),
            StylePropertyInstanceAccessor(borderRadiusBottomRight),
            StylePropertyInstanceAccessor(color),
            StylePropertyInstanceAccessor(fontFamily),
            StylePropertyInstanceAccessor(fontSize),
            StylePropertyInstanceAccessor(fontStyle),
            StylePropertyInstanceAccessor(fontWeight),
            StylePropertyInstanceAccessor(lineHeight),
            StylePropertyInstanceAccessor(maxLines),
            StylePropertyInstanceAccessor(objectFit),
            StylePropertyInstanceAccessor(objectPositionX),
            StylePropertyInstanceAccessor(objectPositionY),
            StylePropertyInstanceAccessor(opacity),
            StylePropertyInstanceAccessor(textAlign),
            StylePropertyInstanceAccessor(textOverflow),
            StylePropertyInstanceAccessor(textTransform),
            StylePropertyInstanceAccessor(tintColor),

            // Unit constants

            StaticValue("UnitPoint", Number::New(env, StyleNumberUnitPoint)),
            StaticValue("UnitPercent", Number::New(env, StyleNumberUnitPercent)),
            StaticValue("UnitViewportWidth", Number::New(env, StyleNumberUnitViewportWidth)),
            StaticValue("UnitViewportHeight", Number::New(env, StyleNumberUnitViewportHeight)),
            StaticValue("UnitViewportMin", Number::New(env, StyleNumberUnitViewportMin)),
            StaticValue("UnitViewportMax", Number::New(env, StyleNumberUnitViewportMax)),
            StaticValue("UnitAuto", Number::New(env, StyleNumberUnitAuto)),
            StaticValue("UnitAnchor", Number::New(env, StyleNumberUnitAnchor)),
            StaticValue("UnitRootEm", Number::New(env, StyleNumberUnitRootEm)),

            // Helper methods

            InstanceMethod(SymbolFor(env, "updateInternalFlags"), &Style::UpdateInternalFlags),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void Style::Init(Napi::Env env) {
    empty = ObjectWrap::Unwrap(Constructor(env).New({}));
    empty->Ref();
    empty->SuppressDestruct();
}

Style* Style::Empty() {
    return empty;
}

void Style::Reset(const YGNodeRef ygNode, const float viewportWidth, const float viewportHeight,
        const int32_t rootFontSize) const {
    ygNode->setStyle(sEmptyStyle);

    for (auto& value : yogaValues) {
        value->Apply(ygNode, viewportWidth, viewportHeight, rootFontSize);
    }
}

void Style::ApplyRootFontSize(const YGNodeRef ygNode, const int32_t rootFontSize) const {
    for (auto& value : yogaValues) {
        value->ApplyRootFontSize(ygNode, rootFontSize);
    }
}

void Style::ApplyViewportSize(const YGNodeRef ygNode,
        const float viewportWidth, const float viewportHeight) const {
    for (auto& value : yogaValues) {
        value->ApplyViewportSize(ygNode, viewportWidth, viewportHeight);
    }
}

void Style::UpdateInternalFlags(const Napi::CallbackInfo& info) {
    this->flags[StyleFlagsBorder] = this->border() || this->borderTop() || this->borderRight()
        || this->borderBottom() || this->borderLeft();

    this->flags[StyleFlagsPadding] = this->padding() || this->paddingTop() || this->paddingRight()
        || this->paddingBottom() || this->paddingLeft();

    this->flags[StyleFlagsBorderRadius] = this->borderRadius() || this->borderRadiusTopLeft()
        || this->borderRadiusTopRight() || this->borderRadiusBottomLeft() || this->borderRadiusBottomRight();

    this->flags[StyleFlagsLayoutOnly] = !this->borderColor() && !this->backgroundColor()
        && this->backgroundImage().empty();

    this->flags[StyleFlagsHasFont] = !this->fontFamily().empty() && this->fontSize();
}

template<typename T>
Value Style::JSGetter(const std::unique_ptr<T>& styleValue, Napi::Env env) const {
    return styleValue ? styleValue->ToJS(env) : env.Undefined();
}

template<typename E, typename T>
void Style::JSSetter(std::unique_ptr<T>& styleValue, Napi::Value jsValue, bool isYogaStyleValue) {
    E e;

    if (!T::ToValue(jsValue, &e)) {
        if (isYogaStyleValue && styleValue) {
            this->yogaValues.erase(
                std::remove(this->yogaValues.begin(), this->yogaValues.end(), styleValue.get()),
                this->yogaValues.end());
        }

        styleValue.reset();
    } else if (styleValue) {
        styleValue->Set(e);
    } else {
        styleValue.reset(new T(e));

        if (isYogaStyleValue) {
            this->yogaValues.push_back(styleValue.get());
        }
    }
}

} // namespace ls
