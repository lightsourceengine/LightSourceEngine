/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "BaseAudioDestination.h"
#include <napi-ext.h>

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Number;
using Napi::NewStringArray;
using Napi::Value;

namespace ls {

Value BaseAudioDestination::GetDecoders(const CallbackInfo& info) {
    return NewStringArray(info.Env(), this->decoders);
}

void BaseAudioDestination::Resume(const CallbackInfo& info) {
}

void BaseAudioDestination::Pause(const CallbackInfo& info) {
}

void BaseAudioDestination::Stop(const CallbackInfo& info) {
}

Value BaseAudioDestination::GetVolume(const CallbackInfo& info) {
    return Number::New(info.Env(), 0);
}

void BaseAudioDestination::SetVolume(const CallbackInfo& info, const Napi::Value& value) {
}

Value BaseAudioDestination::HasCapability(const CallbackInfo& info) {
    return Boolean::New(info.Env(), false);
}

} // namespace ls
