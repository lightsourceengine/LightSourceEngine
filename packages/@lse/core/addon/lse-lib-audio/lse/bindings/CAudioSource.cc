/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "AudioExports.h"

#include <napix.h>
#include <lse/AudioSource.h>

using napix::unwrap_this_as;
using napix::js_class::define;
using napix::descriptor::instance_method;
using napix::descriptor::instance_value;

namespace lse {
namespace bindings {

static napi_value Load(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto source{ci.unwrap_this_as<AudioSource>(env)};

  if (napix::is_string(env, ci[0])) {
    try {
      source->Load(napix::as_string_utf8(env, ci[0]).c_str());
    } catch (const std::exception& e) {
      napix::throw_error(env, e.what());
    }
  } else {
    auto buffer{napix::as_buffer(env, ci[0])};

    if (buffer.empty()) {
      napix::throw_error(env, "invalid buffer");
      return {};
    }

    try {
      source->Load(buffer.data, buffer.size);
    } catch (const std::exception& e) {
      napix::throw_error(env, e.what());
    }
  }

  return {};
}

static napi_value Unload(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioSource>(env, info)->Unload();
  return {};
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioSource>(env, info)->Destroy();
  return {};
}

static napi_value Play(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<2>(env, info)};
  auto source{ci.unwrap_this_as<AudioSource>(env)};
  auto fadeInMs{napix::as_int32(env, ci[0], 0)};
  auto loops{napix::as_int32(env, ci[1], 0)};

  source->Play(fadeInMs < 0 ? 0 : fadeInMs, loops < 0 ? 0 : loops);

  return {};
}

static napi_value HasCapability(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto source{ci.unwrap_this_as<AudioSource>(env)};
  auto capability{napix::as_int32(env, ci[0], -1)};
  bool result{};

  if (IsEnum<AudioSourceCapability>(capability)) {
    result = source->HasCapability(static_cast<AudioSourceCapability>(capability));
  }

  return napix::to_value(env, result);
}

static napi_value GetVolume(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioSource>(env, info)->GetVolume();
  return {};
}

static napi_value SetVolume(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<AudioSource>(env, info)->SetVolume(0);
  return {};
}

napi_value CreateAudioSourceClass(napi_env env, napi_callback constructor, const char* name) noexcept {
  return define(env, name, constructor, {
      instance_method("load", &Load),
      instance_method("unload", &Unload),
      instance_method("destroy", &Destroy),
      instance_method("play", &Play),
      instance_method("hasCapability", &HasCapability),
      instance_method("getVolume", &GetVolume),
      instance_method("setVolume", &SetVolume),
  });
}

} // namespace bindings
} // namespace lse
