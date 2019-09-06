/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "BaseAudioAdapter.h"
#include "napi-ext.h"

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::StringArray;
using Napi::Value;

namespace ls {

Value BaseAudioAdapter::IsAttached(const CallbackInfo& info) {
    return Boolean::New(info.Env(), this->isAttached);
}

Value BaseAudioAdapter::GetAudioDevices(const CallbackInfo& info) {
    return StringArray(info.Env(), this->audioDevices);
}

Value BaseAudioAdapter::CreateSampleAudioDestination(const CallbackInfo& info) {
    return info.Env().Null();
}

Value BaseAudioAdapter::CreateStreamAudioDestination(const CallbackInfo& info) {
    return info.Env().Null();
}

} // namespace ls
