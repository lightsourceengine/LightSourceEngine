/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace ls {

class InputDevice {
 public:
    virtual ~InputDevice() = default;

    int32_t GetId() const noexcept { return this->id; }

    Napi::Value GetId(const Napi::CallbackInfo& info);
    Napi::Value GetUUID(const Napi::CallbackInfo& info);
    Napi::Value GetMapping(const Napi::CallbackInfo& info);
    Napi::Value GetName(const Napi::CallbackInfo& info);

 protected:
    int32_t id{-1};
    std::string uuid{};
    std::string name{};
};

} // namespace ls
