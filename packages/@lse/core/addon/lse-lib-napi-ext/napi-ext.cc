/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "napi-ext.h"

namespace Napi {

constexpr size_t kCopyUtf8BufferSize = 256;
static char sCopyUtf8Buffer[kCopyUtf8BufferSize]{};

Symbol SymbolFor(const Napi::Env& env, const std::string& key) {
  static FunctionReference symbolFor;

  if (symbolFor.IsEmpty()) {
    HandleScope scope(env);
    symbolFor = Napi::Permanent(env.Global().Get("Symbol").As<Object>().Get("for").As<Function>());
  }

  return symbolFor({ String::New(env, key) }).As<Symbol>();
}

std::string ObjectGetString(const Object& object, const std::string& key) {
  if (!object.Has(key)) {
    throw Error::New(object.Env(), "Expected property value to be Object: " + key);
  }

  auto value{ object.Get(key) };

  if (!value.IsString()) {
    throw Error::New(object.Env(), "Expected property value to be String: " + key);
  }

  return value.As<String>();
}

std::string ObjectGetStringOrEmpty(const Object& object, const std::string& key) {
  if (!object.Has(key)) {
    return "";
  }

  auto value{ object.Get(key) };

  if (!value.IsString()) {
    return "";
  }

  return value.As<Napi::String>();
}

bool ObjectGetBooleanOrDefault(const Object& object, const std::string& key, bool defaultValue) {
  if (!object.Has(key)) {
    return defaultValue;
  }

  auto value{ object.Get(key) };

  if (!value.IsBoolean()) {
    return defaultValue;
  }

  return value.As<Boolean>();
}

std::string ToLowerCase(const Napi::String& text) {
  if (text.IsEmpty()) {
    return "";
  }

  auto env{ text.Env() };
  HandleScope scope(env);
  static FunctionReference toLowerCase;

  if (toLowerCase.IsEmpty()) {
    toLowerCase = Napi::Permanent(env.Global()
                                      .Get("String").As<Function>()
                                      .Get("prototype").As<Object>()
                                      .Get("toLowerCase").As<Function>());
  }

  return toLowerCase.Call(text, {}).As<String>();
}

std::string ToUpperCase(const Napi::String& text) {
  if (text.IsEmpty()) {
    return "";
  }

  auto env{ text.Env() };
  HandleScope scope(env);
  static FunctionReference toUpperCase;

  if (toUpperCase.IsEmpty()) {
    toUpperCase = Permanent(env.Global()
                                .Get("String").As<Function>()
                                .Get("prototype").As<Object>()
                                .Get("toUpperCase").As<Function>());
  }

  return toUpperCase.Call(text, {}).As<String>();
}

void Call(const FunctionReference& func, const std::initializer_list<napi_value>& args) {
  if (!func.IsEmpty()) {
    func.Call(args);
  }
}

Value Call(const Napi::Env& env, const FunctionReference& func, const std::initializer_list<napi_value>& args) {
  return func.IsEmpty() ? env.Undefined() : func.Call(args);
}

bool AssignFunctionReference(FunctionReference& ref, const Napi::Value& value) {
  auto env{ value.Env() };
  HandleScope scope(env);

  if (value.IsNull() || value.IsUndefined()) {
    ref.Reset();
  } else if (value.IsFunction()) {
    if (ref.IsEmpty() || !value.StrictEquals(ref.Value())) {
      ref.Reset(value.As<Function>(), 1);
    }
  } else {
    return false;
  }

  return true;
}

ObjectReference Permanent(Object value) {
  ObjectReference ref{ Persistent(value) };

  ref.SuppressDestruct();

  return ref;
}

FunctionReference Permanent(Function value) {
  FunctionReference ref{ Persistent(value) };

  ref.SuppressDestruct();

  return ref;
}

Napi::Value RunScript(const Napi::Env& env, const String& script) {
  napi_value result{ nullptr };
  const auto status{ napi_run_script(env, script, &result) };

  NAPI_THROW_IF_FAILED(
      env,
      status,
      Value());

  return { env, result };
}

Napi::Value RunScript(const Napi::Env& env, const std::string& script) {
  return RunScript(env, String::New(env, script));
}

char* CopyUtf8(const Napi::Value& value, char* buffer, size_t bufferSize) noexcept {
  if (!value.IsEmpty()) {
    napi_status status = napi_get_value_string_utf8(value.Env(), value, buffer, bufferSize, nullptr);

    if (status == napi_ok) {
      return buffer;
    }
  }

  if (buffer && bufferSize > 0) {
    *buffer = '\0';
  }

  return buffer;
}

char* CopyUtf8(const Napi::Value& value) noexcept {
  return CopyUtf8(value, &sCopyUtf8Buffer[0], kCopyUtf8BufferSize);
}

size_t SizeOfCopyUtf8Buffer() noexcept {
  return kCopyUtf8BufferSize;
}

size_t StringByteLength(const Napi::Value& value) noexcept {
  if (!value.IsEmpty()) {
    size_t length{};
    napi_status status = napi_get_value_string_utf8(value.Env(), value, nullptr, 0, &length);

    if (status == napi_ok) {
      return length;
    }
  }

  return 0;
}

bool IsNullish(const Napi::Env& env, const Napi::Value& value) noexcept {
  napi_valuetype type;

  if (napi_typeof(env, value, &type) == napi_ok) {
    return type == napi_null || type == napi_undefined;
  }

  return false;
}

void ObjectFreeze(const Napi::Object& object) {
  auto env{ object.Env() };
  auto objectClass{ env.Global().Get("Object") };

  if (!objectClass.IsFunction()) {
    return;
  }

  auto freeze{ objectClass.As<Napi::Function>().Get("freeze") };

  if (!freeze.IsFunction()) {
    return;
  }

  freeze.As<Napi::Function>().Call({ object });
}

SafeHandleScope::SafeHandleScope(napi_env env) noexcept : env(env), scope(nullptr) {
  napi_open_handle_scope(this->env, &this->scope);
}

SafeHandleScope::~SafeHandleScope() noexcept {
  if (this->scope) {
    napi_close_handle_scope(this->env, this->scope);
  }
}

Napi::Number NewNumber(const Napi::Env& env, uint32_t value) {
  napi_value number;
  napi_status status = napi_create_uint32(env, value, &number);
  NAPI_THROW_IF_FAILED(env, status, Number());
  return { env, number };
}

Napi::Number NewNumber(const Napi::Env& env, int32_t value) {
  napi_value number;
  napi_status status = napi_create_int32(env, value, &number);
  NAPI_THROW_IF_FAILED(env, status, Number());
  return { env, number };
}

Napi::Number NewNumber(const Napi::Env& env, int64_t value) {
  napi_value number;
  napi_status status = napi_create_int32(env, value, &number);
  NAPI_THROW_IF_FAILED(env, status, Number());
  return { env, number };
}

Napi::Number NewNumber(const Napi::Env& env, float value) {
  napi_value number;
  napi_status status = napi_create_double(env, value, &number);
  NAPI_THROW_IF_FAILED(env, status, Number());
  return { env, number };
}

} // namespace Napi
