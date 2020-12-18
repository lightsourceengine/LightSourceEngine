/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/bindings/StyleOps.h>

#include <ls/bindings/Convert.h>
#include <ls/bindings/JSStyleValue.h>
#include <ls/bindings/JSStyleTransformSpec.h>
#include <ls/StyleValidator.h>
#include <ls/string-ext.h>
#include <cassert>

namespace ls {
namespace bindings {

void StyleOps::CreateNative() {
    this->native = std::make_shared<Style>();
}

StyleRef StyleOps::GetNative() const noexcept {
    return this->native;
}

void StyleOps::Set(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };

    NAPI_EXPECT_TRUE(env, info[0].IsNumber(), "style property arg must be a number");

    auto propertyInt{ info[0].As<Napi::Number>().Int32Value() };

    NAPI_EXPECT_TRUE(env, IsValidEnum<StyleProperty>(propertyInt), "invalid style property id");

    this->Set(env, static_cast<StyleProperty>(propertyInt), info[1]);
}

void StyleOps::Set(const Napi::Env& env, StyleProperty property, const Napi::Value& value) {
    assert(this->native != nullptr);
    switch (StylePropertyMetaGetType(property)) {
        case StylePropertyMetaTypeEnum:
            if (value.IsString()) {
                auto stringValue{ ToLowercase(Napi::CopyUtf8(value)) };

                if (StyleValidator::IsValidValue(property, stringValue)) {
                    this->native->SetEnum(property, Napi::CopyUtf8(value));
                } else {
                    this->native->SetUndefined(property);
                }
            } else {
                this->native->SetUndefined(property);
            }
            break;
        case StylePropertyMetaTypeString:
            if (value.IsString()) {
                this->native->SetString(property, value.As<Napi::String>());
            } else {
                this->native->SetUndefined(property);
            }
            break;
        case StylePropertyMetaTypeColor:
            if (Napi::IsNullish(env, value)) {
                this->native->SetUndefined(property);
            } else {
                auto color{ UnboxColor(env, value) };

                if (color.has_value()) {
                    this->native->SetColor(property, *color);
                } else {
                    this->native->SetUndefined(property);
                }
            }
            break;
        case StylePropertyMetaTypeNumber: {
            auto number{ UnboxStyleValue(env, value) };

            if (number.has_value()) {
                if (number->IsUndefined()) {
                    this->native->SetUndefined(property);
                } else if (StyleValidator::IsValidValue(property, *number)) {
                    this->native->SetNumber(property, *number);
                } else {
                    this->native->SetUndefined(property);
                }
            } else {
                this->native->SetUndefined(property);
            }

            break;
        }
        case StylePropertyMetaTypeInteger:
            if (value.IsNumber()) {
                auto intValue{ value.As<Napi::Number>().Int32Value() };

                if (StyleValidator::IsValidValue(property, intValue)) {
                    this->native->SetInteger(property, intValue);
                } else {
                    this->native->SetUndefined(property);
                }
            } else {
                this->native->SetUndefined(property);
            }
            break;
        case StylePropertyMetaTypeTransform:
            this->native->SetTransform(JSStyleTransformSpec::ToStyleTransformSpecList(value));
            break;
        default:
            assert(false);
            throw Napi::Error::New(env, "style property has no type!");
    }
}

Napi::Value StyleOps::Get(StyleProperty property, const Napi::CallbackInfo& info) {
    assert(this->native != nullptr);
    auto env{ info.Env() };
    Napi::HandleScope scope(env);

    switch (StylePropertyMetaGetType(property)) {
        case StylePropertyMetaTypeEnum:
            return Napi::String::New(env, this->native->GetEnumString(property));
        case StylePropertyMetaTypeColor:
            return BoxColor(env, this->native->GetColor(property));
        case StylePropertyMetaTypeString:
            if (this->native->IsEmpty(property)) {
                return env.Undefined();
            } else {
                return Napi::String::New(env, this->native->GetString(property));
            }
        case StylePropertyMetaTypeNumber:
            if (this->native->IsEmpty(property)) {
                return JSStyleValue::Undefined(env);
            } else {
                return JSStyleValue::New(env, this->native->GetNumber(property));
            }
        case StylePropertyMetaTypeTransform:
            if (this->native->IsEmpty(property)) {
                return JSStyleTransformSpec::Undefined(env);
            } else {
                return JSStyleTransformSpec::New(env, this->native->GetTransform());
            }
        case StylePropertyMetaTypeInteger:
            if (this->native->IsEmpty(property)) {
                return env.Undefined();
            } else {
                return Napi::Number::New(env, *this->native->GetInteger(property));
            }
        default:
            assert(false);
            throw Napi::Error::New(env, "style property has no type!");
    }
}

} // namespace bindings
} // namespace ls
