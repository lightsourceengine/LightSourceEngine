/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SafeObjectWrap.h"

namespace Napi {
namespace detail {

napi_value StaticMethodBridge(napi_env env, napi_callback_info info) {
  CallbackInfo callbackInfo(env, info);
  auto callback{ reinterpret_cast<ObjectWrapStaticMethod>(callbackInfo.Data()) };

#ifdef NAPI_CPP_EXCEPTIONS
  try {
    return callback(callbackInfo);
  } catch (const Error& e) {
    e.ThrowAsJavaScriptException();
    return nullptr;
  }
#else
  return callback(callbackInfo);
#endif
}

napi_value StaticVoidMethodBridge(napi_env env, napi_callback_info info) {
  CallbackInfo callbackInfo(env, info);
  auto callback{ reinterpret_cast<ObjectWrapStaticVoidMethod>(callbackInfo.Data()) };

#ifdef NAPI_CPP_EXCEPTIONS
  try {
    callback(callbackInfo);
  } catch (const Error& e) {
    e.ThrowAsJavaScriptException();
  }
#else
  callback(callbackInfo);
#endif

  return nullptr;
}

FunctionReference DefineClass(
    Napi::Env env, const char* utf8name, bool permanent,
    std::vector<napi_property_descriptor>& properties, napi_callback constructorBridge) {
  napi_status status;

  // From ObjectWrap implementation:
  //
  // We must replace static method property descriptors with value property
  // descriptors such that the value is a function-valued `napi_value` created
  // with `CreateFunction()`.
  //
  // This replacement could be made for instance methods as well, but V8 aborts
  // if we do that, because it expects methods defined on the prototype template
  // to have `FunctionTemplate`s.

  for (auto& prop : properties) {
    if (prop.method && (prop.attributes & napi_static)) {
      status = napi_create_function(env, prop.utf8name, NAPI_AUTO_LENGTH, prop.method, prop.data, &(prop.value));
      NAPI_THROW_IF_FAILED(env, status, FunctionReference());
      prop.method = nullptr;
      prop.data = nullptr;
    }
  }

  napi_value constructor;
  status = napi_define_class(env, utf8name, NAPI_AUTO_LENGTH, constructorBridge, nullptr, properties.size(),
                             properties.data(), &constructor);
  NAPI_THROW_IF_FAILED(env, status, FunctionReference());

  FunctionReference ref;

  if (permanent) {
    ref.Reset(Function(env, constructor), 1);
    ref.SuppressDestruct();
  } else {
    ref.Reset(Function(env, constructor));
  }

  return ref;
}

bool EnsureConstructCall(napi_env env, napi_callback_info info) noexcept {
  napi_value newTarget{};
  const auto status{ napi_get_new_target(env, info, &newTarget) };

  if (!newTarget || status != napi_ok) {
    napi_throw_type_error(env, nullptr, "Class constructors cannot be invoked without 'new'");
    return false;
  }

  return true;
}

bool EnsureNativeAlloc(napi_env env, bool allocated) noexcept {
  if (!allocated) {
    napi_throw_type_error(env, nullptr, "Unknown error creating ObjectWrap.");
  }

  return allocated;
}

} // namespace detail
} // namespace Napi