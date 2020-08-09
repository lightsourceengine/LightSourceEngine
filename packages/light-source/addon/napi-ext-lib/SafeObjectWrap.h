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
template <typename T>
using ObjectWrapRemoveRefCallback = void (*)(T*);

/**
 * Helper wrapper for passing property names to the SafeObjectWrap binding methods.
 */
struct PropertyName {
    const char* utf8Name;
    napi_value name;

    PropertyName(const char* utf8Name) noexcept : utf8Name(utf8Name), name(nullptr) {}
    PropertyName(const Symbol& value) noexcept : utf8Name(nullptr), name(value) {}
};

/**
 * SafeObjectWrap reference methods.
 *
 * The reference methods are separated out so the user can create interfaces for
 * SafeObjectWrap subclasses. SafeObjectWrap extends this virtually to allow this, so
 * the interfaces can optionally extend this virtually to access reference methods.
 */
class SafeObjectWrapReference {
 public:
    virtual ~SafeObjectWrapReference() = default;

    Napi::Env Env() const { return this->ref.Env(); }
    Object Value() const { return this->ref.Value(); }
    uint32_t Ref() { return this->ref.Ref(); }
    uint32_t Unref() { return this->ref.Unref(); }
    void SuppressDestruct() { return this->ref.SuppressDestruct(); }

 protected:
    Reference<Object> ref;
};

/**
 * Version of Napi::ObjectWrap class that handles constructor exceptions and reduces some memory & code size
 * overheard.
 *
 * The construct process is broken up into C++ object memory allocation and javascript constructor logic. The logic
 * must be in an overridable Constructor() method. If the constructor throws an exception, the underlying javascript
 * object will be garbage collected appropriately. (ObjectWrap may destroy this object immediately, causing issues
 * with normal javascript constructor exception handling)
 *
 * The class stores native method mappings to a static vtable. This eliminates an allocation per method and
 * uses less memory.
 *
 * @tparam T Derived class
 */
template <typename T>
class SafeObjectWrap : public virtual SafeObjectWrapReference {
 public:
    SafeObjectWrap(const CallbackInfo& info);

    // Create a constructor. The returned FunctionReference is persistent.
    static FunctionReference DefineClass(Napi::Env env, const char* utf8name, bool permanent,
        const std::initializer_list<napi_property_descriptor>& properties);
    static FunctionReference DefineClass(Napi::Env env, const char* utf8name, bool permanent,
        const std::vector<napi_property_descriptor>& properties);

    // Static property declarations.
    static napi_property_descriptor StaticValue(const PropertyName& id, Napi::Value value,
        napi_property_attributes attributes = napi_default) noexcept;
    static napi_property_descriptor StaticMethod(const PropertyName& id, ObjectWrapStaticMethod method,
        napi_property_attributes attributes = napi_default) noexcept;
    static napi_property_descriptor StaticMethod(const PropertyName& id, ObjectWrapStaticVoidMethod method,
        napi_property_attributes attributes = napi_default) noexcept;
    static napi_property_descriptor StaticAccessor(const PropertyName& id, ObjectWrapStaticMethod getter,
        napi_property_attributes attributes = napi_default) noexcept;
    static napi_property_descriptor StaticAccessor(const PropertyName& id, ObjectWrapStaticMethod getter,
        ObjectWrapStaticSetter setter, napi_property_attributes attributes = napi_default) noexcept;

    // Instance property declarations.
    static napi_property_descriptor InstanceValue(const PropertyName& id, Napi::Value value,
        napi_property_attributes attributes = napi_default) noexcept;
    static napi_property_descriptor InstanceMethod(const PropertyName& id, ObjectWrapInstanceMethod<T> method,
        napi_property_attributes attributes = napi_default) noexcept;
    static napi_property_descriptor InstanceMethod(const PropertyName& id, ObjectWrapInstanceVoidMethod<T> method,
        napi_property_attributes attributes = napi_default) noexcept;
    static napi_property_descriptor InstanceAccessor(const PropertyName& id, ObjectWrapInstanceMethod<T> getter,
        napi_property_attributes attributes = napi_default) noexcept;
    static napi_property_descriptor InstanceAccessor(const PropertyName& id, ObjectWrapInstanceGetter<T> getter,
        ObjectWrapInstanceSetter<T> setter, napi_property_attributes attributes = napi_default) noexcept;

    // Cast (or unwrap) a javascript object to it's C++ class instance.
    static T* Cast(const Napi::Value& value) noexcept;

    // Cast (or unwrap) a javascript object to it's C++ class instance. If cast successful, add a reference.
    static T* CastRef(const Napi::Value& value) noexcept;

    /**
     * Safely add a reference on a SafeObjectWrap instance.
     *
     * @param instance The instance to reference
     * @return instance if ref was added; otherwise nullptr is returned
     */
    static T* AddRef(T* instance) noexcept;

    /**
     * Safely call remove reference on a SafeObjectWrap instance.
     *
     * Use case: this->myObject = MyObject::RemoveRef(this->myObject);
     *
     * @param instance The instance to remove reference on. If null, the method is a no-op.
     * @param callback Function called before instance reference is removed.
     * @return always return a nullptr
     */
    static T* RemoveRef(T* instance, ObjectWrapRemoveRefCallback<T> callback = nullptr) noexcept;

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

    static napi_value ConstructorBridge(napi_env env, napi_callback_info info);
    static napi_value InstanceMethodOrGetterBridge(napi_env env, napi_callback_info info);
    static napi_value InstanceVoidMethodBridge(napi_env env, napi_callback_info info);
    static napi_value InstanceSetterBridge(napi_env env, napi_callback_info info);
    static napi_value StaticGetterBridge(napi_env env, napi_callback_info info);
    static napi_value StaticSetterBridge(napi_env env, napi_callback_info info);
    static void* AppendVTableMethod(struct MethodEntry&& method);
    static intptr_t UnwrapVTableIndex(void* data) noexcept;

 private:
    static std::vector<MethodEntry> vtable;
};

namespace detail {

// private methods that do not need to be templated

napi_value StaticMethodBridge(napi_env env, napi_callback_info info);
napi_value StaticVoidMethodBridge(napi_env env, napi_callback_info info);
FunctionReference DefineClass(Napi::Env env, const char* utf8name, bool permanent,
    std::vector<napi_property_descriptor>& properties, napi_callback constructorBridge);
bool EnsureConstructCall(napi_env env, napi_callback_info info) noexcept;
bool EnsureNativeAlloc(napi_env env, bool allocated) noexcept;

} // namespace detail

} // namespace Napi

#include "SafeObjectWrap-inl.h"
