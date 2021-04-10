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

namespace lse {
namespace bindings {

static std::vector<std::string> GetDecoders(napi_env env, napi_value options) noexcept {
  if (!options || napix::is_nullish(env, options)) {
    return {};
  }

  napi_value decoders{};
  if (napi_get_named_property(env, options, "decoders", &decoders) != napi_ok) {
    return {};
  }

  bool isArray{};
  if (napi_is_array(env, decoders, &isArray) != napi_ok || !isArray) {
    return {};
  }

  uint32_t length{};
  if (napi_get_array_length(env, decoders, &length) != napi_ok || length == 0) {
    return {};
  }

  std::vector<std::string> result;

  result.reserve(length);

  for (uint32_t i = 0; i < length; i++) {
    napi_value element{};
    napi_get_element(env, decoders, i, &element);
    auto decoder{ napix::as_string_utf8(env, element) };

    if (!decoder.empty()) {
      result.emplace_back(std::move(decoder));
    }
  }

  return result;
}

AudioPluginConfig ToAudioPluginConfig(napi_env env, napi_value options) noexcept {
  napi_valuetype type{};
  napi_typeof(env, options, &type);

  if (type == napi_object) {
    return {
        napix::object_get(env, options, "audioFormat"),
        napix::object_get_or(env, options, "frequency", -1),
        napix::object_get_or(env, options, "bytesPerSample", -1),
        napix::object_get_or(env, options, "channels", -1),
        GetDecoders(env, options)
    };
  }

  return {};
}

} // namespace bindings
} // namespace lse
