/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "SafeObjectWrap.h"

// Convert std::exception to Napi::Error
#define NAPI_TRY(ENV, EXPR) try { EXPR; } catch (std::exception& e) { throw Napi::Error::New(ENV, e.what()); }
// if expr == true, throw a Napi::Error
#define NAPI_EXPECT_TRUE(ENV, EXPR, MESSAGE) if (!(EXPR)) { throw Napi::Error::New(ENV, MESSAGE); }
// if expr == false, throw a Napi::Error
#define NAPI_EXPECT_FALSE(ENV, EXPR, MESSAGE) if (EXPR) { throw Napi::Error::New(ENV, MESSAGE); }
// if expr is not null, throw a Napi::Error
#define NAPI_EXPECT_NULL(ENV, EXPR, MESSAGE) if ((EXPR) != nullptr) { throw Napi::Error::New(ENV, MESSAGE); }
// if expr is null, throw a Napi::Error
#define NAPI_EXPECT_NOT_NULL(ENV, EXPR, MESSAGE) if ((EXPR) == nullptr) { throw Napi::Error::New(ENV, MESSAGE); }

namespace Napi {

/**
 * Creates a Symbol from a string key using javascript's Symbol.for().
 *
 * @throws Napi::Error generated by accessing the javascript context
 */
Symbol SymbolFor(const Napi::Env& env, const std::string& key);

/**
 * Get a String property value from an Object.
 *
 * @throws Napi::Error if key does not exist in object or keyed property is not a String
 */
std::string ObjectGetString(const Object& object, const std::string& key);

/**
 * Get a String property value from an Object. If the property does not exist, an empty string is returned.
 *
 * @throws Napi::Error generated by accessing the javascript context
 */
std::string ObjectGetStringOrEmpty(const Object& object, const std::string& key);

/**
 * Get a Number property value from an Object. If no property exists for key, defaultValue
 * is returned.
 *
 * @tparam T an integer or floating point type supported by Napi::Number.
 * @throws Napi::Error generated by accessing the javascript context
 */
template<typename T>
T ObjectGetNumberOrDefault(const Object& object, const std::string& key, T defaultValue);
template<typename T>
T ObjectGetNumberOrDefault(const Object& object, uint32_t index, T defaultValue);

/**
 * Get a Boolean property value from an Object. If no property exists for key, defaultValue
 * is returned.
 *
 * @tparam T an integer or floating point type supported by Napi::Number.
 * @throws Napi::Error generated by accessing the javascript context
 */
bool ObjectGetBooleanOrDefault(const Object& object, const std::string& key, bool defaultValue);

/**
 * Call text.toLowerCase().
 */
std::string ToLowerCase(const Napi::String& text);

/**
 * Call text.toUpperCase().
 */
std::string ToUpperCase(const Napi::String& text);

/**
 * Safely call a javascript void function.
 *
 * If the javascript function is not set, the call will be a no-op.
 */
void Call(const Napi::FunctionReference& func, const std::initializer_list<napi_value>& args = {});

/**
 * Safely call a javascript function with a return value.
 *
 * If the javascript function is not set, undefined will be returned.
 */
Napi::Value Call(
    const Napi::Env& env, const Napi::FunctionReference& func,
    const std::initializer_list<napi_value>& args = {});

/**
 * Safely assign a raw value to a FunctionReference.
 *
 * If value is null or undefined, the reference is reset. If the value strictly equals the references value, the
 * reference is unchanged. If the value is a function, it is assigned to a reference. For all other inputs, the
 * reference remains unchanged and false is returned (indicating a failed assignment.
 */
bool AssignFunctionReference(Napi::FunctionReference& ref, const Napi::Value& value);

/**
 * Create an ObjectReference for the lifetime of the application. The reference has a ref count of 1 and the
 * destructor suppressed.
 */
Napi::ObjectReference Permanent(Napi::Object value);

/**
 * Create an FunctionReference for the lifetime of the application. The reference has a ref count of 1 and the
 * destructor suppressed.
 */
Napi::FunctionReference Permanent(Napi::Function value);

/**
 * Evaluate a javascript string.
 */
Napi::Value RunScript(const Napi::Env& env, const std::string& script);

/**
 * Evaluate a javascript string.
 */
Napi::Value RunScript(const Napi::Env& env, const Napi::String& script);

/**
 * Copies a javascript string's raw utf8 encoded bytes to a user supplied buffer.
 *
 * If the source string is longer than bufferSize, the length is truncated by bufferSize - 1 and copied into
 * buffer. The truncation does not consider utf8 encoding, so the truncation could corrupt the string (caller should
 * manage this case).
 *
 * @param value A javascript string.
 * @param buffer Byte buffer to copy into
 * @param bufferSize The size of buffer in bytes
 * @return the buffer argument. If an error occurred or value is not a string, buffer is null terminated and returned.
 */
char* CopyUtf8(const Napi::Value& value, char* buffer, size_t bufferSize) noexcept;

/**
 * Copies a javascript string's raw bytes to an internal buffer.
 *
 * The function limits the copy of raw bytes to 255 bytes. If a larger copy is needed, use CopyUtf8() or
 * String.Utf8Value().
 *
 * The returned char* is owned by the napi-ext library. The user should not delete this pointer. The pointer and it's
 * data are  valid until the next call to CopyUtf8(), which can change the pointer or overwrite data.
 *
 * @param value A javascript string.
 * @return pointer to the cstring buffer. If an error occurred or value is not a string, an empty string is returned.
 */
char* CopyUtf8(const Napi::Value& value) noexcept;

/** @return the number of bytes in CopyUtf8()'s internal memory buffer (should be 256.. 255 + null terminator). */
size_t SizeOfCopyUtf8Buffer() noexcept;

/**
 * Gets the number of utf8 bytes that make up the string.
 *
 * The number of bytes may not equal the number of characters in the string (if the string contains any non-ascii,
 * utf8 characters). The byte length is intended to be used for string memory copy operations.
 *
 * @param value The string value.
 * @return number of raw bytes in the string. If the value is not a string, 0 is returned.
 */
size_t StringByteLength(const Napi::Value& value) noexcept;

/**
 * Common constructor pattern used by SafeObjectWrap subclasses.
 *
 * The constructor is passed an External containing a native factory method. The factory method
 * creates the internal native implementation.
 */
template<typename T, typename F>
T* ConstructorWithExternalFactory(const Napi::CallbackInfo& info, const char* className);

/**
 * @return true if value is null or undefined; otherwise, false (even if value is empty).
 */
bool IsNullish(const Napi::Env& env, const Napi::Value& value) noexcept;

/**
 * Cast a Number value to a primitive integer.
 *
 * @tparam T Must be an integer
 * @param value Number value.
 * @return value cast as primitive; defaultValue if cast fails
 */
template<typename T>
T CastNumberOrDefault(const Napi::Value& value, T defaultValue) noexcept;

/**
 * Convert a vector of C++ primitives to a JS Array.
 *
 * @tparam SourceType C++ primitive type
 * @tparam ToType JS Value that has a constructor that accepts SourceType
 * @param env JS environment
 * @param source input vector
 * @return array
 */
template<typename SourceType, typename ToType>
Napi::Array ToArray(const Napi::Env& env, const std::vector<SourceType>& source);

/**
 * Call Object.freeze() on the passed in object.
 */
void ObjectFreeze(const Napi::Object& object);

/**
 * Implementation of Napi::HandleScope that does not throw in destructor.
 */
class SafeHandleScope {
 public:
  explicit SafeHandleScope(napi_env env) noexcept;
  ~SafeHandleScope() noexcept;

  NAPI_DISALLOW_ASSIGN_COPY(SafeHandleScope)

 private:
  napi_env env;
  napi_handle_scope scope;
};

// The napi Number constructor only supports doubles. Patch in support for other primitives.
Napi::Number NewNumber(const Napi::Env& env, uint32_t value);
Napi::Number NewNumber(const Napi::Env& env, int32_t value);
Napi::Number NewNumber(const Napi::Env& env, int64_t value);
Napi::Number NewNumber(const Napi::Env& env, float value);

} // namespace Napi

#include "napi-ext-inl.h"
