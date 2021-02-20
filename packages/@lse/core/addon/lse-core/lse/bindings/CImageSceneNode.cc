/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CoreClasses.h"

#include <napix.h>
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

  void Invoke(Resource* image, const std::string& errorMessage) override {
    napi_value jsImage;
    napi_value jsErrorMessage{};

    try {
      jsImage = image->Summarize(this->env);
    } catch (const std::exception& e) {
      LOG_ERROR(e.what());
      return;
    }

    if (image->GetState() == Resource::Error) {
      jsErrorMessage = napix::to_value_or_null(this->env, errorMessage.c_str());
    } else {
      napi_get_undefined(env, &jsErrorMessage);
    }

    if (!jsErrorMessage) {
      LOG_ERROR("failed to create message");
      return;
    }

    auto status = napix::call_function(this->env, this->callback, { jsImage, jsErrorMessage }, nullptr);

    if (status != napi_ok) {
      LOG_ERROR("image status callback failed");
    }
  }

 private:
  napi_env env{};
  napi_ref callback{};
};

static napi_value GetSource(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, napix::unwrap_this_as<ImageSceneNode>(env, info)->GetSource());
}

static napi_value SetSource(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto node{ci.unwrap_this_as<ImageSceneNode>(env)};
  std::string src;

  if (napix::is_string(env, ci[0])) {
    src = napix::as_string_utf8(env, ci[0]);
  }

  node->SetSource(env, std::move(src));

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
  props.emplace_back(instance_accessor("src", &GetSource, &SetSource));

  return define(env, NAME, &CSceneNodeConstructor<ImageSceneNode>, props.size(), props.data());
}

} // namespace bindings
} // namespace lse
