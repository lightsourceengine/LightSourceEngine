/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include "CoreClasses.h"

#include <napix.h>
#include <lse/Log.h>
#include <lse/Image.h>
#include <lse/ImageSceneNode.h>
#include <lse/bindings/CSceneNodeConstructor.h>

using napix::js_class::define;
using napix::descriptor::instance_accessor;
using napix::descriptor::instance_value;
using napix::descriptor::instance_method;

namespace lse {
namespace bindings {

class NapiImageStatusCallback : public ImageStatusCallback {
 public:
  NapiImageStatusCallback(napi_env env, napi_ref callback) noexcept : env(env), callback(callback) {
  }

  ~NapiImageStatusCallback() override {
    if (this->callback) {
      napi_delete_reference(this->env, this->callback);
    }
  }

  void Invoke(Image* image) override {
    napi_value jsImage{napix::to_value_or_null(this->env, image->GetId())};
    napi_value jsErrorMessage{};

    // TODO: convert image to javascript

    if (image->IsError()) {
      jsErrorMessage = napix::to_value_or_null(this->env, image->GetErrorMessage().c_str());
    } else {
      napi_get_undefined(this->env, &jsErrorMessage);
    }

    if (!jsImage || !jsErrorMessage) {
      LOG_ERROR("failed to args");
      return;
    }

    auto callStatus = napix::call_function(this->env, this->callback, { jsImage, jsErrorMessage }, nullptr);

    if (napix::has_pending_exception(env)) {
      LOG_ERROR("Uncaught JS exception: %s", napix::pop_pending_exception(env));
    } else if (callStatus != napi_ok) {
      LOG_ERROR("callback invoke: %i", callStatus);
    }
  }

 private:
  napi_env env{};
  napi_ref callback{};
};

static napi_value SetSource(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<ImageSceneNode>(env)};

  if (napix::is_nullish(env, ci[0])) {
    node->ResetSource();
  } else {
    ImageRequest request{
      napix::object_get(env, ci[0], "uri"),
      napix::object_get_or(env, ci[0], "width", 0),
      napix::object_get_or(env, ci[0], "height", 0),
    };

    node->SetSource(request);
  }

  return {};
}

static napi_value SetImageStatusCallback(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<ImageSceneNode>(env)};
  std::unique_ptr<NapiImageStatusCallback> callback;

  if (napix::is_function(env, ci[0])) {
    if (node->HasImageStatusCallback()) {
      napix::throw_error(env, "callback already set");
      return {};
    }

    napi_ref ref{};
    napi_create_reference(env, ci[0], 1, &ref);

    if (!ref) {
      napix::throw_error(env, "failed to create ref");
      return {};
    }

    callback = std::make_unique<NapiImageStatusCallback>(env, ref);
  } else if (!napix::is_nullish(env, ci[0])) {
    napix::throw_error(env, "expected function or null");
    return {};
  }

  if (napi_set_named_property(env, ci.thisContext, "cb", napix::to_value(env, callback != nullptr)) == napi_ok) {
    node->SetImageStatusCallback(std::move(callback));
  }

  return {};
}

napi_value CImageSceneNode::CreateClass(napi_env env) noexcept {
  auto props{ CSceneNode::GetClassProperties(env) };

  // Flag that allows js to avoid calling into native code.
  props.emplace_back(instance_value("cb", napix::to_value(env, false), napi_writable));
  props.emplace_back(instance_method("setCallback", &SetImageStatusCallback));
  props.emplace_back(instance_method("setSource", &SetSource));

  return define(env, NAME, &CSceneNodeConstructor<ImageSceneNode>, props.size(), props.data());
}

} // namespace bindings
} // namespace lse
