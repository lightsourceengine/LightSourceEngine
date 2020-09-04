/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

namespace Napi {

template <typename T>
std::vector<typename SafeObjectWrap<T>::MethodEntry> SafeObjectWrap<T>::vtable = SafeObjectWrap<T>::CreateVTable();

template <typename T>
SafeObjectWrap<T>::SafeObjectWrap(const CallbackInfo &info) {
}

template <typename T>
SafeObjectWrap<T>::~SafeObjectWrap() {
}

template <typename T>
T* SafeObjectWrap<T>::Cast(const Napi::Value& value) noexcept {
    if (value.IsObject()) {
        auto env{ value.Env() };
        HandleScope scope(env);
        auto object{ value.As<Object>() };
        void* instance;

        if (object.InstanceOf(T::GetClass(env)) && napi_unwrap(value.Env(), value, &instance) == napi_ok) {
            return static_cast<T*>(instance);
        }
    }

    return nullptr;
}

template <typename T>
T* SafeObjectWrap<T>::CastRef(const Napi::Value& value) noexcept {
    return AddRef(T::Cast(value));
}

template <typename T>
T* SafeObjectWrap<T>::StaticCast(const Napi::Value& value) noexcept {
    if (value.IsObject()) {
        void* instance;

        if (napi_unwrap(value.Env(), value, &instance) == napi_ok) {
            return static_cast<T*>(instance);
        }
    }

    return nullptr;
}

template <typename T>
T* SafeObjectWrap<T>::AddRef(T* instance) noexcept {
    if (instance) {
        try {
            instance->Ref();

            return instance;
        } catch (std::exception& e) {
            // TODO: log warning?
        }
    }

    return nullptr;
}

template <typename T>
T* SafeObjectWrap<T>::RemoveRef(T* instance, ObjectWrapRemoveRefCallback<T> callback) noexcept {
    if (instance) {
        if (callback) {
            try {
                callback(instance);
            } catch (std::exception& e) {
                // TODO: log warning?
            }
        }

        try {
            instance->Unref();
        } catch (std::exception& e) {
            // TODO: log warning?
        }
    }
    return nullptr;
}

template <typename T>
napi_value SafeObjectWrap<T>::ConstructorBridge(napi_env env, napi_callback_info info) {
    if (!detail::EnsureConstructCall(env, info)) {
        return nullptr;
    }

    const CallbackInfo callbackInfo(env, info);
    T* instance;

#ifdef NAPI_CPP_EXCEPTIONS
    try {
        instance = new T(callbackInfo);
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
    } catch (...) {
        instance = nullptr;
    }
#else
    instance = new T(callbackInfo);
#endif

    if (!detail::EnsureNativeAlloc(env, instance)) {
        return nullptr;
    }

    auto finalizer = [](napi_env /*env*/, void* data, void* /*hint*/) {
        delete static_cast<T*>(data);
    };

    napi_ref reference;
    const auto status{ napi_wrap(callbackInfo.Env(), callbackInfo.This(), instance, finalizer, nullptr, &reference) };
    NAPI_THROW_IF_FAILED_VOID(callbackInfo.Env(), status);

    instance->ref = Reference<Object>(callbackInfo.Env(), reference);

#ifdef NAPI_CPP_EXCEPTIONS
    try {
        instance->Constructor(callbackInfo);
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
    }
#else
    instance->Constructor(callbackInfo);
#endif

    return callbackInfo.This();
}

template <typename T>
napi_value SafeObjectWrap<T>::StaticGetterBridge(napi_env env, napi_callback_info info) {
    const CallbackInfo callbackInfo(env, info);
    auto vtableIndex{ UnwrapVTableIndex(callbackInfo.Data()) };

#ifdef NAPI_CPP_EXCEPTIONS
    try {
#endif
        return vtable[vtableIndex].staticGetterMethod(callbackInfo);

#ifdef NAPI_CPP_EXCEPTIONS
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    }
#endif
}

template <typename T>
napi_value SafeObjectWrap<T>::StaticSetterBridge(napi_env env, napi_callback_info info) {
    const CallbackInfo callbackInfo(env, info);
    auto vtableIndex{ UnwrapVTableIndex(callbackInfo.Data()) };

#ifdef NAPI_CPP_EXCEPTIONS
    try {
#endif
        vtable[vtableIndex + 1].staticSetterMethod(callbackInfo, callbackInfo[0]);
        return nullptr;

#ifdef NAPI_CPP_EXCEPTIONS
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    }
#endif
}

template <typename T>
napi_value SafeObjectWrap<T>::InstanceMethodOrGetterBridge(napi_env env, napi_callback_info info) {
    const CallbackInfo callbackInfo(env, info);
    auto vtableIndex{ UnwrapVTableIndex(callbackInfo.Data()) };
    auto instance{ T::StaticCast(callbackInfo.This()) };

#ifdef NAPI_CPP_EXCEPTIONS
    try {
#endif
        return (instance->*(vtable[vtableIndex].instanceMethod))(callbackInfo);

#ifdef NAPI_CPP_EXCEPTIONS
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    }
#endif
}

template <typename T>
napi_value SafeObjectWrap<T>::InstanceVoidMethodBridge(napi_env env, napi_callback_info info) {
    const CallbackInfo callbackInfo(env, info);
    auto vtableIndex{ UnwrapVTableIndex(callbackInfo.Data()) };
    auto instance{ T::StaticCast(callbackInfo.This()) };

#ifdef NAPI_CPP_EXCEPTIONS
    try {
#endif
        (instance->*(vtable[vtableIndex].instanceVoidMethod))(callbackInfo);
        return nullptr;

#ifdef NAPI_CPP_EXCEPTIONS
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    }
#endif
}

template <typename T>
napi_value SafeObjectWrap<T>::InstanceSetterBridge(napi_env env, napi_callback_info info) {
    const CallbackInfo callbackInfo(env, info);
    auto vtableIndex{ UnwrapVTableIndex(callbackInfo.Data()) + 1 };
    auto instance{ T::StaticCast(callbackInfo.This()) };

#ifdef NAPI_CPP_EXCEPTIONS
    try {
#endif
        (instance->*(vtable[vtableIndex].instanceSetterMethod))(callbackInfo, callbackInfo[0]);
        return nullptr;

#ifdef NAPI_CPP_EXCEPTIONS
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    }
#endif
}

template <typename T>
napi_value SafeObjectWrap<T>::InstanceGetterFunctionBridge(napi_env env, napi_callback_info info) {
    const CallbackInfo callbackInfo(env, info);
    auto vtableIndex{ UnwrapVTableIndex(callbackInfo.Data()) };
    auto instance{ T::StaticCast(callbackInfo.This()) };

#ifdef NAPI_CPP_EXCEPTIONS
    try {
#endif
        return vtable[vtableIndex].instanceGetterFunction(instance, callbackInfo);

#ifdef NAPI_CPP_EXCEPTIONS
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    }
#endif
}

template <typename T>
napi_value SafeObjectWrap<T>::InstanceSetterFunctionBridge(napi_env env, napi_callback_info info) {
    const CallbackInfo callbackInfo(env, info);
    auto vtableIndex{ UnwrapVTableIndex(callbackInfo.Data()) + 1 };
    auto instance{ T::StaticCast(callbackInfo.This()) };

#ifdef NAPI_CPP_EXCEPTIONS
    try {
#endif
        vtable[vtableIndex].instanceSetterFunction(instance, callbackInfo, callbackInfo[0]);
        return nullptr;

#ifdef NAPI_CPP_EXCEPTIONS
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    }
#endif
}

template <typename T>
FunctionReference SafeObjectWrap<T>::DefineClass(Napi::Env env, const char* utf8name, bool permanent,
        const std::vector<napi_property_descriptor>& properties) {
    std::vector<napi_property_descriptor> props;

    for (auto& p : properties) {
        props.push_back(p);
    }

    return detail::DefineClass(env, utf8name, permanent, props, T::ConstructorBridge);
}

template <typename T>
FunctionReference SafeObjectWrap<T>::DefineClass(Napi::Env env, const char* utf8name, bool permanent,
        const std::initializer_list<napi_property_descriptor>& properties) {
    std::vector<napi_property_descriptor> props;

    for (auto& p : properties) {
        props.push_back(p);
    }

    return detail::DefineClass(env, utf8name, permanent, props, T::ConstructorBridge);
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::StaticValue(const PropertyName& id, Napi::Value value,
                                                  napi_property_attributes attributes) noexcept {
    return InstanceValue(id, value, static_cast<napi_property_attributes>(attributes | napi_static));
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::StaticMethod(const PropertyName& id, ObjectWrapStaticMethod method,
                                                   napi_property_attributes attributes) noexcept {
    return {
        id.utf8Name,
        id.name,
        detail::StaticMethodBridge,
        nullptr,
        nullptr,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        reinterpret_cast<void*>(method)
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::StaticMethod(const PropertyName& id, ObjectWrapStaticVoidMethod method,
        napi_property_attributes attributes) noexcept {
    return {
        id.utf8Name,
        id.name,
        detail::StaticVoidMethodBridge,
        nullptr,
        nullptr,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        reinterpret_cast<void*>(method)
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::StaticAccessor(const PropertyName& id, ObjectWrapStaticMethod getter,
        napi_property_attributes attributes) noexcept {
    return {
        id.utf8Name,
        id.name,
        nullptr,
        StaticGetterBridge,
        nullptr,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        AppendVTableMethod({ getter })
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::StaticAccessor(const PropertyName& id, ObjectWrapStaticMethod getter,
        ObjectWrapStaticSetter setter, napi_property_attributes attributes) noexcept {
    // Note: Setter stored immediately after getter. Store the index of the getter in data. StaticSetterBridge
    // will +1 on the stored index to access the setter method pointer.
    auto index{ AppendVTableMethod({ getter }) };

    AppendVTableMethod({ setter });

    return {
        id.utf8Name,
        id.name,
        nullptr,
        StaticGetterBridge,
        StaticSetterBridge,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        index
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::InstanceValue(const PropertyName& id, Napi::Value value,
        napi_property_attributes attributes) noexcept {
    return {
        id.utf8Name,
        id.name,
        nullptr,
        nullptr,
        nullptr,
        value,
        attributes,
        nullptr
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::InstanceMethod(const PropertyName& id,
       ObjectWrapInstanceMethod<T> method, napi_property_attributes attributes) noexcept {
    return {
        id.utf8Name,
        id.name,
        InstanceMethodOrGetterBridge,
        nullptr,
        nullptr,
        nullptr,
        attributes,
        AppendVTableMethod({ method })
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::InstanceMethod(const PropertyName& id,
        ObjectWrapInstanceVoidMethod<T> method, napi_property_attributes attributes) noexcept {
    return {
        id.utf8Name,
        id.name,
        InstanceVoidMethodBridge,
        nullptr,
        nullptr,
        nullptr,
        attributes,
        AppendVTableMethod({ method })
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::InstanceAccessor(const PropertyName& id,
        ObjectWrapInstanceGetter<T> getter, napi_property_attributes attributes) noexcept {
    return {
        id.utf8Name,
        id.name,
        nullptr,
        InstanceMethodOrGetterBridge,
        nullptr,
        nullptr,
        attributes,
        AppendVTableMethod({ getter })
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::InstanceAccessor(const PropertyName& id, ObjectWrapInstanceGetter<T> getter,
         ObjectWrapInstanceSetter<T> setter, napi_property_attributes attributes) noexcept {
    // Note: Setter stored immediately after getter. Store the index of the getter in data. InstanceSetterBridge
    // will +1 on the stored index to access the setter method pointer.
    auto index{ AppendVTableMethod({ getter }) };

    AppendVTableMethod({ setter });

    return {
        id.utf8Name,
        id.name,
        nullptr,
        InstanceMethodOrGetterBridge,
        InstanceSetterBridge,
        nullptr,
        attributes,
        index
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::InstanceAccessor(
        const PropertyName& id,
        ObjectWrapInstanceGetterFunction<T> getter,
        napi_property_attributes attributes) noexcept {
    return {
        id.utf8Name,
        id.name,
        nullptr,
        InstanceGetterFunctionBridge,
        nullptr,
        nullptr,
        attributes,
        AppendVTableMethod({ getter })
    };
}

template <typename T>
napi_property_descriptor SafeObjectWrap<T>::InstanceAccessor(
        const PropertyName& id,
        ObjectWrapInstanceGetterFunction<T> getter,
        ObjectWrapInstanceSetterFunction<T> setter,
        napi_property_attributes attributes) noexcept {
    // Note: Setter stored immediately after getter. Store the index of the getter in data. InstanceSetterBridge
    // will +1 on the stored index to access the setter method pointer.
    auto index{ AppendVTableMethod({ getter }) };

    if (setter) {
        AppendVTableMethod({setter});
    }

    return {
        id.utf8Name,
        id.name,
        nullptr,
        InstanceGetterFunctionBridge,
        setter ? InstanceSetterFunctionBridge : nullptr,
        nullptr,
        attributes,
        index
    };
}

template <typename T>
void* SafeObjectWrap<T>::AppendVTableMethod(MethodEntry&& method) {
    vtable.emplace_back(method);
    // return index of new vtable method, but wrapped as a void* to store in the data field
    return reinterpret_cast<void*>(static_cast<intptr_t>(vtable.size() - 1));
}

template <typename T>
intptr_t SafeObjectWrap<T>::UnwrapVTableIndex(void* data) noexcept {
    return reinterpret_cast<intptr_t>(data);
}

template <typename T>
std::vector<typename SafeObjectWrap<T>::MethodEntry> SafeObjectWrap<T>::CreateVTable() {
    decltype(vtable) v;

    v.reserve(8);

    return v;
}

} // namespace Napi
