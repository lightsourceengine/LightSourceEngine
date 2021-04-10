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
