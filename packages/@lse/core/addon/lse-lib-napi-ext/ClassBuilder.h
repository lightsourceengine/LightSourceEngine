/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi.h>
#include "Property.h"

namespace Napi {

using ClassBuilderMethod = Napi::Value (*)(const CallbackInfo&);
using ClassBuilderVoidMethod = void (*)(const CallbackInfo&);

/**
 * Creates javascript bindings for C++ classes. Alternative to Napi::ObjectWrap.
 */
class ClassBuilder {
 public:
  ClassBuilder(const Napi::Env& env, const char* className);

  // Add a static method to the prototype.
  ClassBuilder& WithStaticMethod(
      const Napi::PropertyName& id, ClassBuilderMethod method,
      napi_property_attributes attributes = napi_default);

  // Add a static void method to the prototype.
  ClassBuilder& WithStaticMethod(
      const Napi::PropertyName& id, ClassBuilderVoidMethod method,
      napi_property_attributes attributes = napi_default);

  // Add a static value to the prototype.
  ClassBuilder& WithStaticValue(
      const Napi::PropertyName& id, const Napi::Value& value,
      napi_property_attributes attributes = napi_default);

  // Add a static integer to the prototype.
  ClassBuilder& WithStaticValue(
      const Napi::PropertyName& id, int32_t value,
      napi_property_attributes attributes = napi_default);

  // Create a class constructor Function from the current state of the ClassBuilder.
  Napi::Function ToConstructor();

  // Create a class constructor functionReference from the current state of the ClassBuilder. If permanent is true,
  // the reference has 1 ref and the ref destructor is suppressed.
  Napi::FunctionReference ToConstructorReference(bool permanent);

 private:
  Napi::Env env;
  const char* className;
  napi_callback constructorNative;
  std::vector<napi_property_descriptor> properties;
};

} // namespace Napi
