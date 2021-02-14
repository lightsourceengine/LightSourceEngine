/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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
