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

namespace lse {
namespace bindings {

napi_value ParseColor(napi_env env, napi_callback_info info) noexcept;
napi_value ParseValue(napi_env env, napi_callback_info info) noexcept;

napi_value LoadSDLPlugin(napi_env env, napi_callback_info) noexcept;
napi_value LoadRefPlugin(napi_env env, napi_callback_info) noexcept;
napi_value LoadSDLAudioPlugin(napi_env env, napi_callback_info) noexcept;
napi_value LoadSDLMixerPlugin(napi_env env, napi_callback_info) noexcept;

napi_value LockStyle(napi_env env, napi_callback_info info) noexcept;
napi_value SetStyleParent(napi_env env, napi_callback_info info) noexcept;
napi_value ResetStyle(napi_env env, napi_callback_info info) noexcept;
napi_value InstallStyleValue(napi_env env, napi_callback_info info) noexcept;
napi_value InstallStyleTransformSpec(napi_env env, napi_callback_info info) noexcept;
napi_value InstallImage(napi_env env, napi_callback_info info) noexcept;

napi_value GetSceneNodeInstanceCount(napi_env env, napi_callback_info info) noexcept;

} // namespace bindings
} // namespace lse
