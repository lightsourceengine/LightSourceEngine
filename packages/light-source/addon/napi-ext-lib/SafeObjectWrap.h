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
    virtual ~SafeObjectWrap() = default;

    // Create a constructor. The returned FunctionReference is persistent.
    static FunctionReference DefineClass(Napi::Env env, const char* utf8name,
        const std::initializer_list<PropertyDescriptor>& properties);
    static FunctionReference DefineClass(Napi::Env env, const char* utf8name,
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
    // Connectors between the napi C API and the SafeObjectWrap instance.
    static napi_value ConstructorBridge(napi_env env, napi_callback_info info);
    static napi_value InstanceGetterBridge(napi_env env, napi_callback_info info);
    static napi_value InstanceSetterBridge(napi_env env, napi_callback_info info);
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

    constexpr PropertyName(const char* utf8Name) noexcept : utf8Name(utf8Name), name(nullptr) {}
    constexpr PropertyName(const Symbol& value) noexcept : utf8Name(nullptr), name(value) {}
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
FunctionReference DefineClass(Napi::Env env, const char* utf8name, std::vector<napi_property_descriptor>& properties,
        napi_callback constructorBridge);
napi_value StaticGetterBridge(napi_env env, napi_callback_info info);
napi_value StaticSetterBridge(napi_env env, napi_callback_info info);
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
SafeObjectWrap<T>::SafeObjectWrap(const CallbackInfo &info) {
    auto finalizer = [](napi_env /*env*/, void* data, void* /*hint*/) {
        delete dynamic_cast<T*>(static_cast<SafeObjectWrapBase*>(data));
    };

    napi_ref reference;
    const auto status{
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
napi_value SafeObjectWrap<T>::InstanceGetterBridge(napi_env env, napi_callback_info info) {
    const CallbackInfo callbackInfo(env, info);
    auto data{ static_cast<detail::InstanceProperty<T>*>(callbackInfo.Data()) };
    auto instance{QueryInterface<T>(callbackInfo.This()) };

    if (!instance) {
        return nullptr;
    }

#ifdef NAPI_CPP_EXCEPTIONS
    try {
        return data->getter(instance, callbackInfo);
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    }
#else
    return data->getter(instance, callbackInfo);
#endif
}

template <typename T>
napi_value SafeObjectWrap<T>::InstanceSetterBridge(napi_env env, napi_callback_info info) {
    const CallbackInfo callbackInfo(env, info);
    auto property{ static_cast<detail::InstanceProperty<T>*>(callbackInfo.Data()) };
    auto instance{QueryInterface<T>(callbackInfo.This()) };

    if (!instance) {
        return nullptr;
    }

#ifdef NAPI_CPP_EXCEPTIONS
    try {
        property->setter(instance, callbackInfo, callbackInfo[0]);
    } catch (const Error& e) {
        e.ThrowAsJavaScriptException();
        return nullptr;
    }
#else
    property->setter(instance, callbackInfo, callbackInfo[0]);
#endif

    return nullptr;
}

template <typename T>
FunctionReference SafeObjectWrap<T>::DefineClass(Napi::Env env, const char* utf8name,
        const std::initializer_list<PropertyDescriptor>& properties) {
    auto props{ detail::TransformPropertyDescriptor(properties, properties.size()) };

    return detail::DefineClass(env, utf8name, props, T::ConstructorBridge);
}

template <typename T>
FunctionReference SafeObjectWrap<T>::DefineClass(Napi::Env env, const char* utf8name,
        const std::vector<PropertyDescriptor>& properties) {
    auto props{ detail::TransformPropertyDescriptor(properties, properties.size()) };

    return detail::DefineClass(env, utf8name, props, T::ConstructorBridge);
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
        detail::StaticMethodBridge,
        nullptr,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        reinterpret_cast<void*>(getter)
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::StaticAccessor(const PropertyName& id, ObjectWrapStaticMethod getter,
        ObjectWrapStaticSetter setter, napi_property_attributes attributes) noexcept {
    auto data{ std::make_unique<detail::StaticProperty>(getter, setter) };

    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        nullptr,
        detail::StaticGetterBridge,
        detail::StaticSetterBridge,
        nullptr,
        static_cast<napi_property_attributes>(attributes | napi_static),
        static_cast<void *>(data.release())
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
    auto data{ std::make_unique<detail::InstanceProperty<T>>(
        [method](T* instance, const CallbackInfo& info) -> napi_value {
            return (instance->*method)(info);
        },
        nullptr)
    };

    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        InstanceGetterBridge,
        nullptr,
        nullptr,
        nullptr,
        attributes,
        static_cast<void*>(data.release())
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::InstanceMethod(const PropertyName& id,
        ObjectWrapInstanceVoidMethod<T> method, napi_property_attributes attributes) noexcept {
    auto data{ std::make_unique<detail::InstanceProperty<T>>(
        [method](T* instance, const CallbackInfo& info) -> napi_value {
            (instance->*method)(info);
            return nullptr;
        },
        nullptr)
    };

    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        InstanceGetterBridge,
        nullptr,
        nullptr,
        nullptr,
        attributes,
        static_cast<void*>(data.release())
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::InstanceAccessor(const PropertyName& id,
        ObjectWrapInstanceMethod<T> getter, napi_property_attributes attributes) noexcept {
    auto data = std::make_unique<detail::InstanceProperty<T>>(
        [getter](T* instance, const CallbackInfo& info) -> napi_value {
            return (instance->*getter)(info);
        },
        nullptr);

    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        nullptr,
        InstanceGetterBridge,
        nullptr,
        nullptr,
        attributes,
        static_cast<void*>(data.release())
    });
}

template <typename T>
PropertyDescriptor SafeObjectWrap<T>::InstanceAccessor(
        const PropertyName& id,
        ObjectWrapInstanceGetter<T> getter,
        ObjectWrapInstanceSetter<T> setter,
        napi_property_attributes attributes) noexcept {
    auto data = std::make_unique<detail::InstanceProperty<T>>(
        [getter](T* instance, const CallbackInfo& info) -> napi_value {
            return (instance->*getter)(info);
        },
        [setter](T* instance, const CallbackInfo& info, const Napi::Value& value) {
            (instance->*setter)(info, value);
        });

    return PropertyDescriptor({
        id.utf8Name,
        id.name,
        nullptr,
        InstanceGetterBridge,
        InstanceSetterBridge,
        nullptr,
        attributes,
        static_cast<void*>(data.release())
    });
}

} // namespace Napi
