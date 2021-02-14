/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <node_api.h>
#include <lse/AudioPluginConfig.h>

namespace lse {
namespace bindings {

napi_value CreateAudioPluginClass(napi_env env, napi_callback constructor, const char* name) noexcept;
napi_value CreateAudioDestinationClass(napi_env env, napi_callback constructor, const char* name) noexcept;
napi_value CreateAudioSourceClass(napi_env env, napi_callback constructor, const char* name) noexcept;
AudioPluginConfig ToAudioPluginConfig(napi_env env, napi_value options) noexcept;

} // namespace bindings
} // namespace lse
