/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/bindings/JSStyleValue.h>

#include <lse/bindings/Convert.h>

static Napi::FunctionReference jsStyleValueClass;
static Napi::ObjectReference jsStyleValueUndefined;

namespace lse {
namespace bindings {

Napi::Function JSStyleValue::GetClass(const Napi::Env& env) {
    if (jsStyleValueClass.IsEmpty()) {
        const char* script = R"(
            const StyleValue = class StyleValue {
                constructor(value, unit) {
                    this.unit = unit || 0
                    this.value = value || 0
                    Object.freeze(this)
                }

                isUndefined () {
                    return this.unit === 0
                }
            }

            /*return*/ StyleValue
        )";

        auto result{ Napi::RunScript(env, script) };

        if (!result.IsFunction()) {
            throw Napi::Error::New(env, "Failed to parse StyleValue class source.");
        }

        // Patch in a a static of(value) method to create StyleValue objects. This is the same conversion logic
        // used to parse style number properties
        auto function = result.As<Napi::Function>();

        function.DefineProperty(Napi::PropertyDescriptor::Function(
            "of",
            [](const Napi::CallbackInfo& info) -> Napi::Value {
                auto env = info.Env();
                const auto& styleValue = UnboxStyleValue(env, info[0]);

                return styleValue.has_value() ? JSStyleValue::New(env, *styleValue) : JSStyleValue::Undefined(env);
            },
            napi_default,
            nullptr));

        jsStyleValueClass.Reset(function, 1);
        jsStyleValueClass.SuppressDestruct();
    }

    return jsStyleValueClass.Value();
}

Napi::Object JSStyleValue::New(const Napi::Env& env, const StyleValue& value) {
    auto styleValueClass{ GetClass(env) };

    return styleValueClass.New({ Napi::Number::New(env, value.value), Napi::Number::New(env, value.unit) });
}

Napi::Object JSStyleValue::Undefined(const Napi::Env& env) {
    if (jsStyleValueUndefined.IsEmpty()) {
        jsStyleValueUndefined.Reset(New(env, { StyleNumberUnitUndefined, 0 }), 1);
        jsStyleValueUndefined.SuppressDestruct();
    }

    return jsStyleValueUndefined.Value();
}

StyleValue JSStyleValue::ToStyleValue(const Napi::Value& value) {
    if (!value.IsObject()) {
        return StyleValue::OfUndefined();
    }

    auto jsStyleValue{ value.As<Napi::Object>() };

    if (!jsStyleValue.InstanceOf(JSStyleValue::GetClass(value.Env()))) {
        return StyleValue::OfUndefined();
    }

    auto unitProp{ Napi::ObjectGetNumberOrDefault(jsStyleValue, "unit", -1) };
    auto valueProp{ Napi::ObjectGetNumberOrDefault(jsStyleValue, "value", 0.f) };

    if (!IsValidEnum<StyleNumberUnit>(unitProp)) {
        return StyleValue::OfUndefined();
    }

    return { static_cast<StyleNumberUnit>(unitProp), valueProp };
}

} // namespace bindings
} // namespace lse
