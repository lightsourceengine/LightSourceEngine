/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "ObjectBuilder.h"
#include "napi-ext.h"
#include <cassert>

namespace Napi {

ObjectBuilder::ObjectBuilder(const Napi::Env& env, void* data) noexcept : env(env), data(data) {
}

ObjectBuilder& ObjectBuilder::WithMethod(const Napi::PropertyName& id, napi_callback method) {
  static_assert(noexcept(method), "method must be declared with noexcept");

  return this->SetPropertyDescriptor({
    id.utf8Name,
    id.name,
    method,
    nullptr,
    nullptr,
    nullptr,
    napi_default,
    this->data
  });
}

ObjectBuilder& ObjectBuilder::WithValue(
    const Napi::PropertyName& id, int32_t value, napi_property_attributes attributes) {
  return WithValue(id, Napi::Number::New(this->env, value), attributes);
}

ObjectBuilder& ObjectBuilder::WithValue(
    const Napi::PropertyName& id, const char* value, napi_property_attributes attributes) {
  return WithValue(id, Napi::String::New(this->env, value), attributes);
}

ObjectBuilder& ObjectBuilder::WithProperty(const Napi::PropertyName& id, napi_callback getter, napi_callback setter) {
  static_assert(noexcept(getter), "property getter must be declared with noexcept");
  static_assert(noexcept(setter), "property getter must be declared with noexcept");

  return this->SetPropertyDescriptor({
    id.utf8Name,
    id.name,
    nullptr,
    getter,
    setter,
    nullptr,
    napi_writable,
    this->data
  });
}

ObjectBuilder& ObjectBuilder::WithValue(
    const Napi::PropertyName& id, const Napi::Value& value, napi_property_attributes attributes) {
  return this->SetPropertyDescriptor({
      id.utf8Name,
      id.name,
      nullptr,
      nullptr,
      nullptr,
      value,
      napi_default,
      nullptr
  });
}

ObjectBuilder& ObjectBuilder::SetPropertyDescriptor(const napi_property_descriptor& descriptor) {
  napi_status status = napi_define_properties(this->env, this->GetObject(), 1, &descriptor);

  NAPI_THROW_IF_FAILED(this->env, status, *this);

  return *this;
}

Napi::Object ObjectBuilder::ToObject() {
  auto obj{ this->GetObject() };

  if (this->freeze) {
    Napi::ObjectFreeze(obj);
  }

  this->object = {};

  return obj;
}

ObjectBuilder& ObjectBuilder::Freeze() {
  this->freeze = true;

  return *this;
}

Napi::Object ObjectBuilder::GetObject() {
  if (this->object.IsEmpty()) {
    this->object = Napi::Object::New(this->env);
  }
  return this->object;
}

} // namespace Napi
