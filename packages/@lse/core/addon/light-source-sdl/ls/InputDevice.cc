/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/InputDevice.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::String;
using Napi::Value;

namespace ls {

Value InputDevice::GetId(const CallbackInfo& info) {
    return Number::New(info.Env(), this->id);
}

Value InputDevice::GetUUID(const CallbackInfo& info) {
    return String::New(info.Env(), this->uuid);
}

Value InputDevice::GetMapping(const CallbackInfo& info) {
    return info.Env().Undefined();
}

Value InputDevice::GetName(const CallbackInfo& info) {
    return String::New(info.Env(), this->name);
}

} // namespace ls
