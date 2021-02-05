/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace lse {
namespace bindings {

Napi::Value ParseColor(const Napi::CallbackInfo& info);
Napi::Object GetStyleProperties(Napi::Env env);

Napi::Value LoadSDLPlugin(const Napi::CallbackInfo& info);
Napi::Value LoadSDLAudioPlugin(const Napi::CallbackInfo& info);
Napi::Value LoadSDLMixerPlugin(const Napi::CallbackInfo& info);

Napi::Value CreateRefGraphicsContext(const Napi::CallbackInfo& info);
Napi::Value CreateSceneComposite(const Napi::CallbackInfo& info);
Napi::Value CreateStageComposite(const Napi::CallbackInfo& info);
Napi::Value CreateFontManagerComposite(const Napi::CallbackInfo& info);

} // namespace bindings
} // namespace lse
