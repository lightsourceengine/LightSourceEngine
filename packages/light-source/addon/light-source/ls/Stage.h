/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>

#include "Resources.h"
#include "ThreadPool.h"

namespace ls {

class Stage : public Napi::SafeObjectWrap<Stage> {
 public:
    explicit Stage(const Napi::CallbackInfo& info);
    virtual ~Stage() = default;

    ThreadPool* GetThreadPool() const noexcept { return &this->threadPool; }
    Resources* GetResources() const noexcept { return &this->resources; }

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    Napi::Value GetResourceDomainPath(const Napi::CallbackInfo& info);
    void SetResourceDomainPath(const Napi::CallbackInfo& info, const Napi::Value& value);
    void Destroy(const Napi::CallbackInfo& info);

 private:
    mutable ThreadPool threadPool;
    mutable Resources resources;
    std::string resourceDomainPath;

    friend Napi::SafeObjectWrap<Stage>;
};

} // namespace ls
