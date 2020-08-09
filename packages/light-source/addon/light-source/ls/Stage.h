/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/Resources.h>
#include <ls/ThreadPool.h>

namespace ls {

class Stage : public Napi::SafeObjectWrap<Stage> {
 public:
    explicit Stage(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<Stage>(info) {}
    ~Stage() override = default;

    static Napi::Function GetClass(Napi::Env env);
    void Destroy(const Napi::CallbackInfo& info);

    ThreadPool* GetThreadPool() const noexcept { return &this->threadPool; }
    Resources* GetResources() const noexcept { return &this->resources; }

 private:
    mutable ThreadPool threadPool;
    mutable Resources resources;
};

} // namespace ls
