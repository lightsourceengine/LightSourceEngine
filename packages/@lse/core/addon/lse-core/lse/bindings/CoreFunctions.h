/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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
