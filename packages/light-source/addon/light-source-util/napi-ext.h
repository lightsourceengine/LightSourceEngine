/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace Napi {

/**
 * Creates a Symbol from a string key using javascript's Symbol.for().
 *
 * @throws Napi::Error generated by accessing the javascript context
 */
Symbol SymbolFor(Napi::Env env, const char* key);

/**
 * Looks up a property in a javascript Object, returning it's std::String value.
 *
 * @throws Napi::Error if key does not exist in object or keyed property is not a String
 */
std::string ObjectGetString(const Object& object, const char* key);

/**
 * Looks up a property in a javascript Object, returning it's std::String value. If no property exists for key, an
 * empty std::string is returned.
 *
 * @throws Napi::Error generated by accessing the javascript context
 */
std::string ObjectGetStringOrEmpty(const Object& object, const char* key);

/**
 * Looks up a property in a javascript Object, returning it's number value. If no property exists for key, defaultValue
 * is returned.
 *
 * @tparam T an integer or floating point type supported by Napi::Number.
 * @throws Napi::Error generated by accessing the javascript context
 */
template<typename T>
T ObjectGetNumberOrDefault(const Object& object, const char* key, T defaultValue);

/**
 * Call text.toLowerCase().
 */
std::string ToLowerCase(Napi::Env env, Napi::String text);

/**
 * Call text.toLowerCase().
 */
std::string ToUpperCase(Napi::Env env, Napi::String text);

/**
 * Create a javascript Array of Strings from an iterable list of C++ strings.
 */
template<typename Iterable>
Napi::Array StringArray(Napi::Env env, const Iterable& iterable);

} // namespace Napi

#include "napi-ext-inl.h"
