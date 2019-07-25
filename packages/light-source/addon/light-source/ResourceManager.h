/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace ls {

class ResourceManager : public Napi::ObjectWrap<ResourceManager> {
 public:
    explicit ResourceManager(const Napi::CallbackInfo& info);
    virtual ~ResourceManager() = default;

    static Napi::Function Constructor(Napi::Env env);
};

} // namespace ls
