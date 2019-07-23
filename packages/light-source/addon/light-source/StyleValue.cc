/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "StyleValue.h"

using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::Value;

namespace lse {

void YGStyleNullSetAuto(const YGNodeRef node) {}
void YGStyleNullSetAuto(const YGNodeRef node, const YGEdge) {}
void YGStyleNullSetValue(const YGNodeRef node, const float) {}
void YGStyleNullSetValue(const YGNodeRef node, const YGEdge, const float) {}

bool StyleColorValue::ToValue(Napi::Value value, int64_t* result) {
    if (value.IsNumber()) {
        *result = value.As<Number>();
        return true;
    }

    return false;
}

Value StyleNumberValue::ToJS(Napi::Env env) const {
    auto object = Object::New(env);

    object["unit"] = Number::New(env, this->number.unit);
    object["value"] = Number::New(env, this->number.value);

    return object;
}

bool StyleNumberValue::ToValue(Napi::Value value, StyleNumber* result) {
    HandleScope scope(value.Env());
    auto holder = value.As<Napi::Array>();

    if (holder.Length() != 2) {
        return false;
    }

    auto unitValue = holder.Get(0u);

    if (!unitValue.IsNumber()) {
        return false;
    }

    auto unitValueInt = unitValue.As<Number>().Int32Value();

    if (!IsEnum<StyleNumberUnit>(unitValueInt)) {
        return false;
    }

    auto floatValue = holder.Get(1u);

    if (!floatValue.IsNumber()) {
        return false;
    }

    result->unit = static_cast<StyleNumberUnit>(unitValueInt);
    result->value = floatValue.As<Number>();

    return true;
}

} // namespace lse
