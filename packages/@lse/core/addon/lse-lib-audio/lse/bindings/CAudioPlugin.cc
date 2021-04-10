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

#include "AudioExports.h"

#include <napix.h>
#include <lse/AudioPlugin.h>
#include <lse/Habitat.h>

using napix::unwrap_this_as;
using napix::js_class::define;
using napix::descriptor::instance_method;
using napix::descriptor::instance_value;

namespace lse {
namespace bindings {

static napi_value Attach(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioPlugin>(env, info)->Attach();
  return {};
}

static napi_value Detach(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioPlugin>(env, info)->Detach();
  return {};
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioPlugin>(env, info)->Destroy();
  return {};
}

static napi_value IsAttached(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, unwrap_this_as<AudioPlugin>(env, info)->IsAttached());
}

static napi_value GetDevices(napi_env env, napi_callback_info info) noexcept {
  auto devices{unwrap_this_as<AudioPlugin>(env, info)->GetDevices()};

  return napix::array_from<const std::string&>(env, devices, napix::to_value);
}

static napi_value CreateStreamAudioDestination(napi_env env, napi_callback_info info) noexcept {
  auto constructor{Habitat::GetClass(env, Habitat::Class::CStreamAudioDestination)};

  napi_value instance{};
  napi_new_instance(env, constructor, 0, nullptr, &instance);

  return instance;
}

static napi_value CreateSampleAudioDestination(napi_env env, napi_callback_info info) noexcept {
  auto constructor{Habitat::GetClass(env, Habitat::Class::CSampleAudioDestination)};

  napi_value instance{};
  napi_new_instance(env, constructor, 0, nullptr, &instance);

  return instance;
}

napi_value CreateAudioPluginClass(napi_env env, napi_callback constructor, const char* name) noexcept {
  return define(env, name, constructor, {
    instance_value("type", napix::to_value(env, "audio")),
    instance_method("attach", &Attach),
    instance_method("detach", &Detach),
    instance_method("destroy", &Destroy),
    instance_method("isAttached", &IsAttached),
    instance_method("getDevices", &GetDevices),
    instance_method("createStreamAudioDestination", &CreateStreamAudioDestination),
    instance_method("createSampleAudioDestination", &CreateSampleAudioDestination)
  });
}

} // namespace bindings
} // namespace lse
