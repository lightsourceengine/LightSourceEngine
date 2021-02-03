/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi.h>
#include "Property.h"

namespace Napi {

/**
 * Builds a javascript Object instance.
 *
 * ObjectBuilder adds the following:
 *
 * - less verbose object creation compared to Object
 * - reduced code size from templates
 * - removes function call wrapper for methods and property getter/setter
 */
class ObjectBuilder final {
 public:
  ObjectBuilder(const Napi::Env& env, void* data = nullptr) noexcept;

  ObjectBuilder& WithMethod(const Napi::PropertyName& id, napi_callback method);

  ObjectBuilder& WithValue(
      const Napi::PropertyName& id, napi_value value,
      napi_property_attributes attributes = napi_default);

  ObjectBuilder& WithValue(
      const Napi::PropertyName& id, int32_t value,
      napi_property_attributes attributes = napi_default);

  ObjectBuilder& WithValue(
      const Napi::PropertyName& id, const char* value,
      napi_property_attributes attributes = napi_default);

  ObjectBuilder& WithProperty(const Napi::PropertyName& id, napi_callback getter, napi_callback setter);

  ObjectBuilder& Freeze();

  Napi::Object ToObject();

 private:
  ObjectBuilder& SetPropertyDescriptor(const napi_property_descriptor& descriptor);
  Napi::Object GetObject();

 private:
  Napi::Env env;
  void* data;
  Napi::Object object{};
  bool freeze{};
};

} // namespace Napi
