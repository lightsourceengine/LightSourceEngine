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
#include <lse/AudioDestination.h>
#include <lse/Habitat.h>

using napix::unwrap_this_as;
using napix::js_class::define;
using napix::descriptor::instance_method;
using napix::descriptor::instance_value;

namespace lse {
namespace bindings {

static napi_value CreateAudioSource(napi_env env, napi_callback_info info) noexcept {
  auto self{napix::get_this(env, info)};
  napi_value constructor{};

  if (Habitat::InstanceOf(env, self, Habitat::Class::CSampleAudioDestination)) {
    constructor = Habitat::GetClass(env, Habitat::Class::CSampleAudioSource);
  } else if (Habitat::InstanceOf(env, self, Habitat::Class::CStreamAudioDestination)) {
    constructor = Habitat::GetClass(env, Habitat::Class::CStreamAudioSource);
  }

  if (!constructor) {
    return {};
  }

  napi_value instance{};
  napi_new_instance(env, constructor, 0, nullptr, &instance);

  return instance;
}

static napi_value Resume(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioDestination>(env, info)->Resume();
  return {};
}

static napi_value Pause(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioDestination>(env, info)->Pause();
  return {};
}

static napi_value Stop(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto dest{ci.unwrap_this_as<AudioDestination>(env)};
  auto fadeOutMs{napix::as_int32(env, ci[0], 0)};

  dest->Stop(fadeOutMs < 0 ? 0 : fadeOutMs);

  return {};
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioDestination>(env, info)->Destroy();
  return {};
}

static napi_value HasCapability(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto dest{ci.unwrap_this_as<AudioDestination>(env)};
  auto capability{napix::as_int32(env, ci[0], -1)};
  bool result{};

  if (IsEnum<AudioDestinationCapability>(capability)) {
    result = dest->HasCapability(static_cast<AudioDestinationCapability>(capability));
  }

  return napix::to_value(env, result);
}

static napi_value GetVolume(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioDestination>(env, info)->GetVolume();
  return {};
}

static napi_value SetVolume(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioDestination>(env, info)->GetVolume();
  return {};
}

static napi_value GetDecoders(napi_env env, napi_callback_info info) noexcept {
  auto dest{unwrap_this_as<AudioDestination>(env, info)};

  return napix::array_from<const std::string&>(env, dest->GetDecoders(), napix::to_value);
}

napi_value CreateAudioDestinationClass(napi_env env, napi_callback constructor, const char* name) noexcept {
  return define(env, name, constructor, {
      instance_method("createAudioSource", &CreateAudioSource),
      instance_method("resume", &Resume),
      instance_method("pause", &Pause),
      instance_method("stop", &Stop),
      instance_method("destroy", &Destroy),
      instance_method("hasCapability", &HasCapability),
      instance_method("getVolume", &GetVolume),
      instance_method("setVolume", &SetVolume),
      instance_method("getDecoders", &GetDecoders)
  });
}

} // namespace bindings
} // namespace lse
