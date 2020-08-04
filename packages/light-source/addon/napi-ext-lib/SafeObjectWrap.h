/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <algorithm>
#include <iterator>
#include <vector>

namespace Napi {

using ObjectWrapStaticMethod = Napi::Value (*)(const CallbackInfo&);
using ObjectWrapStaticVoidMethod = void (*)(const CallbackInfo&);
using ObjectWrapStaticGetter = Napi::Value (*)(const CallbackInfo&);
using ObjectWrapStaticSetter = void (*)(const CallbackInfo&, const Napi::Value&);

template <typename T>
using ObjectWrapInstanceVoidMethod = void (T::*)(const CallbackInfo& info);
template <typename T>
using ObjectWrapInstanceMethod = Napi::Value (T::*)(const CallbackInfo& info);
template <typename T>
using ObjectWrapInstanceGetter = Napi::Value (T::*)(const CallbackInfo& info);
template <typename T>
using ObjectWrapInstanceSetter = void (T::*)(const CallbackInfo& info, const Napi::Value& value);

struct PropertyName;

/**
 * Polymorphic base class for SafeObjectWrap and SafeObjectWrap user defined interfaces.
 */
class SafeObjectWrapBase {
 public:
    virtual ~SafeObjectWrapBase() = default;

    Napi::Env Env() const { return this->ref.Env(); }
    Object Value() const { return this->ref.Value(); }
    uint32_t Ref() { return this->ref.Ref(); }
    uint32_t Unref() { return this->ref.Unref(); }
    void SuppressDestruct() { return this->ref.SuppressDestruct(); }

 protected:
    Reference<Object> ref;
};

/**
 * Re-imagined Napi::ObjectWrap class that handles constructor exceptions and dynamic casting of the underlying
 * native pointer.
 *
 * The construct process is broken up into C++ object memory allocation and javascript constructor logic. The logic
 * must be in an overridable Constructor() method. If the constructor throws an exception, the underlying javascript
 * object will be garbage collected appropriately. (ObjectWrap may destroy this object immediately, causing issues
 * with normal javascript cosntructor exception handling)
 *
 * Light Source Engine has many plugin-like javascript objects and native users may not have access to the concrete
 * derived type. The plugin-like objects are accessed through interfaces at the native layer. SafeObjectWrap can
 * dynamically cast the derived class to an interface. ObjectWrap assumes the user is working with the derived class
 * and further assumes you know the exact type.
 *
 * @tparam T Derived class
 */
template <typename T>
class SafeObjectWrap : virtual public SafeObjectWrapBase {
 public:
    SafeObjectWrap(const CallbackInfo& info);

    // Create a constructor. The returned FunctionReference is persistent.
    static FunctionReference DefineClass(Napi::Env env, const char* utf8name, bool permanent,
        const std::initializer_list<PropertyDescriptor>& properties);
    static FunctionReference DefineClass(Napi::Env env, const char* utf8name, bool permanent,
        const std::vector<PropertyDescriptor>& properties);

    // Static property declarations.
    static PropertyDescriptor StaticValue(const PropertyName& id, Napi::Value value,
        napi_property_attributes attributes = napi_default) noexcept;
    static PropertyDescriptor StaticMethod(const PropertyName& id, ObjectWrapStaticMethod method,
        napi_property_attributes attributes = napi_default) noexcept;
    static PropertyDescriptor StaticMethod(const PropertyName& id, ObjectWrapStaticVoidMethod method,
        napi_property_attributes attributes = napi_default) noexcept;
    static PropertyDescriptor StaticAccessor(const PropertyName& id, ObjectWrapStaticMethod getter,
        napi_property_attributes attributes = napi_default) noexcept;
    static PropertyDescriptor StaticAccessor(const PropertyName& id, ObjectWrapStaticMethod getter,
        ObjectWrapStaticSetter setter, napi_property_attributes attributes = napi_default) noexcept;

    // Instance property declarations.
    static PropertyDescriptor InstanceValue(const PropertyName& id, Napi::Value value,
        napi_property_attributes attributes = napi_default) noexcept;
    static PropertyDescriptor InstanceMethod(const PropertyName& id, ObjectWrapInstanceMethod<T> method,
        napi_property_attributes attributes = napi_default) noexcept;
    static PropertyDescriptor InstanceMethod(const PropertyName& id, ObjectWrapInstanceVoidMethod<T> method,
        napi_property_attributes attributes = napi_default) noexcept;
    static PropertyDescriptor InstanceAccessor(const PropertyName& id, ObjectWrapInstanceMethod<T> getter,
        napi_property_attributes attributes = napi_default) noexcept;
    static PropertyDescriptor InstanceAccessor(const PropertyName& id, ObjectWrapInstanceGetter<T> getter,
        ObjectWrapInstanceSetter<T> setter, napi_property_attributes attributes = napi_default) noexcept;

 protected:
    // Optional override for constructor logic. Do not put logic into the C++ constructor!
    virtual void Constructor(const CallbackInfo& info) {}

 private:
    struct MethodEntry {
        MethodEntry() {}
        MethodEntry(ObjectWrapInstanceMethod<T> method) : instanceMethod(method) {}
        MethodEntry(ObjectWrapInstanceVoidMethod<T> method) : instanceVoidMethod(method) {}
        MethodEntry(ObjectWrapInstanceSetter<T> method) : instanceSetterMethod(method) {}
        MethodEntry(ObjectWrapStaticGetter method) : staticGetterMethod(method) {}
        MethodEntry(ObjectWrapStaticSetter method) : staticSetterMethod(method) {}

        union {
            ObjectWrapInstanceMethod<T> instanceMethod;
            ObjectWrapInstanceVoidMethod<T> instanceVoidMethod;
            ObjectWrapInstanceSetter<T> instanceSetterMethod;

            ObjectWrapStaticGetter staticGetterMethod;
            ObjectWrapStaticSetter staticSetterMethod;
            // Note: static methods are put directly into the data field, as they do not need state.
        };
    };

    static std::vector<MethodEntry> vtable;

 private:
    static napi_value ConstructorBridge(napi_env env, napi_callback_info info);
    static napi_value InstanceMethodOrGetterBridge(napi_env env, napi_callback_info info);
    static napi_value InstanceVoidMethodBridge(napi_env env, napi_callback_info info);
    static napi_value InstanceSetterBridge(napi_env env, napi_callback_info info);
    static napi_value StaticGetterBridge(napi_env env, napi_callback_info info);
    static napi_value StaticSetterBridge(napi_env env, napi_callback_info info);
    static void* AppendVTableMethod(MethodEntry&& method) {
        vtable.emplace_back(method);
        // return index of new vtable method, but wrapped as a void* to store in the data field
        return reinterpret_cast<void*>(static_cast<intptr_t>(vtable.size() - 1));
    }
    static intptr_t UnwrapVTableIndex(void* data) noexcept { return reinterpret_cast<intptr_t>(data); }
};

/**
 * Get the native object ptr from a javascript object created with SafeObjectWrap.
 *
 * @tparam I Interface to dynamically cast to.
 * @param value Javascript object. Should be Object, otherwise nullptr is returned.
 * @return The native object dynamically casted to I; otherwise, nullptr.
 */
template<typename I>
I* QueryInterface(Napi::Value value) noexcept;

/**
 * Argument wrapper for a string or Symbol property name.
 */
struct PropertyName {
    const char* utf8Name;
    napi_value name;

    PropertyName(const char* utf8Name) noexcept : utf8Name(utf8Name), name(nullptr) {}
    PropertyName(const Symbol& value) noexcept : utf8Name(nullptr), name(value) {}
};

namespace detail {

struct StaticProperty {
    constexpr StaticProperty(ObjectWrapStaticGetter g, ObjectWrapStaticSetter s) : getter(g), setter(s) {}

    ObjectWrapStaticGetter getter;
    ObjectWrapStaticSetter setter;
};

template <typename I>
struct InstanceProperty {
    constexpr InstanceProperty(std::function<napi_value(I*, const Napi::CallbackInfo&)>&& g,
        std::function<void(I*, const Napi::CallbackInfo&, const Napi::Value&)>&& s) : getter(g), setter(s) {}

    std::function<napi_value(I*, const Napi::CallbackInfo&)> getter;
    std::function<void(I*, const Napi::CallbackInfo&, const Napi::Value&)> setter;
};

SafeObjectWrapBase* Unwrap(Napi::Value wrapper) noexcept;
napi_value StaticMethodBridge(napi_env env, napi_callback_info info);
napi_value StaticVoidMethodBridge(napi_env env, napi_callback_info info);
FunctionReference DefineClass(Napi::Env env, const char* utf8name, bool permanent,
    std::vector<napi_property_descriptor>& properties, napi_callback constructorBridge);
bool EnsureConstructCall(napi_env env, napi_callback_info info) noexcept;
bool EnsureNativeAlloc(napi_env env, bool allocated) noexcept;

template<typename Iterable>
std::vector<napi_property_descriptor> TransformPropertyDescriptor(const Iterable& i, std::size_t len) {
    std::vector<napi_property_descriptor> props;

    props.reserve(len);
    std::transform(i.begin(), i.end(), std::back_inserter(props),
        [](const PropertyDescriptor& p) -> napi_property_descriptor { return p; });

    return props;
}

} // namespace detail

template <typename T>
std::vector<typename SafeObjectWrap<T>::MethodEntry> SafeObjectWrap<T>::vtable(8);

template <typename T>
SafeObjectWrap<T>::SafeObjectWrap(const CallbackInfo &info) {
    auto finalizer = [](napi_env /*env*/, void* data, void* /*hint*/) {
        delete dynamic_cast<T*>(static_cast<SafeObjectWrapBase*>(data));
    };

    napi_ref reference;
    const auto status{
        // cast to SafeObjectWrapBase and convert to void*. this is so QueryInterface can dynamic_cast from
        // a known class: SafeObjectWrapBase.
        napi_wrap(info.Env(), info.This(), static_cast<SafeObjectWrapBase*>(this), finalizer, nullptr, &reference)
    };
    NAPI_THROW_IF_FAILED_VOID(info.Env(), status);

    this->ref = Reference<Object>(info.Env(), reference);
}

template <typename T>
napi_value SafeObjectWrap<T>::ConstructorBridge(napi_env env, napi_callback_info info) {
    if (!detail::EnsureConstructCall(env, info)) {
        return nullptr;
    }

    const CallbackInfo callbackInfo(env, info);
    T* instance{ nullptr };

#ifdef NAPI_CPP_EXCEPTIONS
    try {
        instance = new T(callbackInfo);
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    } catch (...) {
        instance = nullptr;
    }
#else
    instance = new T(callbackInfo);
#endif

    if (!detail::EnsureNativeAlloc(env, instance)) {
        return nullptr;
    }

#ifdef NAPI_CPP_EXCEPTIONS
    try {
        instance->Constructor(callbackInfo);
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
    }
#else
    instance->Constuct(callbackInfo);
#endif

    return callbackInfo.This();
}

template<typename I>
I* QueryInterface(Napi::Value value) noexcept {
    return dynamic_cast<I*>(detail::Unwrap(value));
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
    auto instance{ QueryInterface<T>(callbackInfo.This()) };

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
    auto instance{ QueryInterface<T>(callbackInfo.This()) };

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
    auto instance{ QueryInterface<T>(callbackInfo.This()) };

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
FunctionReference SafeObjectWrap<T>::DefineClass(Napi::Env env, const char* utf8name, bool permanent,
        const std::initializer_list<PropertyDescriptor>& properties) {
    auto props{ detail::TransformPropertyDescriptor(properties, properties.size()) };

    return detail::DefineClass(env, utf8name, permanent, props, T::ConstructorBridge);
}

template <typename T>
FunctionReference SafeObjectWrap<T>::DefineClass(Napi::Env env, const char* utf8name, bool permanent,
        const std::vector<PropertyDescriptor>& properties) {
    auto props{ detail::TransformPropertyDescriptor(properties, properties.size()) };

    return detail::DefineClass(env, utf8name, permanent, props, T::ConstructorBridge);
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::StaticValue(const PropertyName& id, Napi::Value value,
                                                  napi_property_attributes attributes) noexcept {
    return InstanceValue(id, value, static_cast<napi_property_attributes>(attributes | napi_static));
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::StaticMethod(const PropertyName& id, ObjectWrapStaticMethod method,
                                                   napi_property_attributes attributes) noexcept {
    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        detail::StaticMethodBridge,
        nullptr,
        nullptr,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        reinterpret_cast<void*>(method)
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::StaticMethod(const PropertyName& id, ObjectWrapStaticVoidMethod method,
                                                   napi_property_attributes attributes) noexcept {
    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        detail::StaticVoidMethodBridge,
        nullptr,
        nullptr,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        reinterpret_cast<void*>(method)
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::StaticAccessor(const PropertyName& id, ObjectWrapStaticMethod getter,
                                                     napi_property_attributes attributes) noexcept {
    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        nullptr,
        StaticGetterBridge,
        nullptr,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        AppendVTableMethod({ getter })
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::StaticAccessor(const PropertyName& id, ObjectWrapStaticMethod getter,
        ObjectWrapStaticSetter setter, napi_property_attributes attributes) noexcept {
    // Note: Setter stored immediately after getter. Store the index of the getter in data. StaticSetterBridge
    // will +1 on the stored index to access the setter method pointer.
    auto index{ AppendVTableMethod({ getter }) };

    AppendVTableMethod({ setter });

    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        nullptr,
        StaticGetterBridge,
        StaticSetterBridge,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        index
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::InstanceValue(const PropertyName& id, Napi::Value value,
                                                    napi_property_attributes attributes) noexcept {
    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        nullptr,
        nullptr,
        nullptr,
        value,
        attributes,
        nullptr
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::InstanceMethod(const PropertyName& id, ObjectWrapInstanceMethod<T> method,
                                                     napi_property_attributes attributes) noexcept {
    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        InstanceMethodOrGetterBridge,
        nullptr,
        nullptr,
        nullptr,
        attributes,
        AppendVTableMethod({ method })
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::InstanceMethod(const PropertyName& id,
        ObjectWrapInstanceVoidMethod<T> method, napi_property_attributes attributes) noexcept {
    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        InstanceVoidMethodBridge,
        nullptr,
        nullptr,
        nullptr,
        attributes,
        AppendVTableMethod({ method })
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::InstanceAccessor(const PropertyName& id,
        ObjectWrapInstanceMethod<T> getter, napi_property_attributes attributes) noexcept {
    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        nullptr,
        InstanceMethodOrGetterBridge,
        nullptr,
        nullptr,
        attributes,
        AppendVTableMethod({ getter })
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::InstanceAccessor(
        const PropertyName& id,
        ObjectWrapInstanceGetter<T> getter,
        ObjectWrapInstanceSetter<T> setter,
        napi_property_attributes attributes) noexcept {
    // Note: Setter stored immediately after getter. Store the index of the getter in data. InstanceSetterBridge
    // will +1 on the stored index to access the setter method pointer.
    auto index{ AppendVTableMethod({ getter }) };

    AppendVTableMethod({ setter });

    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        nullptr,
        InstanceMethodOrGetterBridge,
        InstanceSetterBridge,
        nullptr,
        attributes,
        index
    });
}

} // namespace Napi
