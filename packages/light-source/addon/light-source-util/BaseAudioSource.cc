/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "BaseAudioSource.h"

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Number;
using Napi::Value;

namespace ls {

Napi::Value BaseAudioSource::GetVolume(const CallbackInfo& info) {
    return Number::New(info.Env(), 0);
}

void BaseAudioSource::SetVolume(const CallbackInfo& info, const Napi::Value& value) {
}

void BaseAudioSource::Play(const CallbackInfo& info) {
}

Value BaseAudioSource::HasCapability(const CallbackInfo& info) {
    return Boolean::New(info.Env(), false);
}

} // namespace ls
