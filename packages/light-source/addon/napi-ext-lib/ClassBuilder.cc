/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "ClassBuilder.h"
#include <cassert>

namespace Napi {

static napi_value StaticMethodBinding(napi_env env, napi_callback_info info);
static napi_value StaticVoidMethodBinding(napi_env env, napi_callback_info info);
static napi_value EmptyConstructor(napi_env env, napi_callback_info info);
static Napi::Function CreateFunction(const Napi::PropertyName& name, napi_env env, napi_callback binding, void* data);

ClassBuilder::ClassBuilder(const Napi::Env& env, const char* className)
: env(env), className(className), constructorNative(&EmptyConstructor) {
}

ClassBuilder& ClassBuilder::WithStaticMethod(
        const Napi::PropertyName& id, ClassBuilderMethod method, napi_property_attributes attributes) {
    // Note: bug in v8 does not allow static method properties, so put the function into a property value
    return WithStaticValue(
        id,
        CreateFunction(id, this->env, &StaticMethodBinding, reinterpret_cast<void*>(method)),
        attributes);
}

ClassBuilder& ClassBuilder::WithStaticMethod(
        const Napi::PropertyName& id, ClassBuilderVoidMethod method, napi_property_attributes attributes) {
    // Note: bug in v8 does not allow static method properties, so put the function into a property value
    return WithStaticValue(
        id,
        CreateFunction(id, this->env, &StaticVoidMethodBinding, reinterpret_cast<void*>(method)),
        attributes);
}

ClassBuilder& ClassBuilder::WithStaticValue(
        const Napi::PropertyName& id, int32_t value, napi_property_attributes attributes) {
    return WithStaticValue(id, Napi::Number::New(this->env, value), attributes);
}

ClassBuilder& ClassBuilder::WithStaticValue(
        const Napi::PropertyName& id, const Napi::Value& value, napi_property_attributes attributes) {
    this->properties.push_back({
        id.utf8Name,
        id.name,
        nullptr,
        nullptr,
        nullptr,
        value,
        static_cast<napi_property_attributes>(attributes | napi_static),
        nullptr
    });

    return *this;
}

Napi::Function ClassBuilder::ToConstructor() {
    napi_value constructor;
    napi_status status = napi_define_class(
        this->env,
        this->className,
        strlen(this->className),
        this->constructorNative,
        nullptr,
        this->properties.size(),
        this->properties.data(),
        &constructor);

    NAPI_THROW_IF_FAILED(this->env, status, Napi::Function());

    return { this->env, constructor };
}

Napi::FunctionReference ClassBuilder::ToConstructorReference(bool permanent) {
    Napi::FunctionReference ref;
    auto constructor{ ToConstructor() };

    if (permanent) {
        ref.Reset(constructor, 1);
        ref.SuppressDestruct();
    } else {
        ref.Reset(constructor);
    }

    return ref;
}

static napi_value StaticMethodBinding(napi_env env, napi_callback_info info) {
    Napi::CallbackInfo cbi(env, info);

    assert(cbi.Data());
    return reinterpret_cast<ClassBuilderMethod>(cbi.Data())(cbi);
}

static napi_value StaticVoidMethodBinding(napi_env env, napi_callback_info info) {
    Napi::CallbackInfo cbi(env, info);

    assert(cbi.Data());
    reinterpret_cast<ClassBuilderMethod>(cbi.Data())(cbi);

    return nullptr;
}

static Napi::Function CreateFunction(const Napi::PropertyName& name, napi_env env, napi_callback binding, void* data) {
    napi_value func;

    auto status = napi_create_function(env, name.utf8Name, strlen(name.utf8Name), binding, data, &func);
    NAPI_THROW_IF_FAILED(env, status, Napi::Function());

    return { env, func };
}

static napi_value EmptyConstructor(napi_env env, napi_callback_info info) {
    napi_value self{};
    napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr);

    return self;
}

} // namespace Napi
