/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include "FontStore.h"
#include "Executor.h"
#include "TaskQueue.h"
#include "Resources.h"

namespace ls {

class StageAdapter;
class AudioAdapter;

class Stage : public Napi::SafeObjectWrap<Stage> {
 public:
    explicit Stage(const Napi::CallbackInfo& info);
    virtual ~Stage() = default;

    FontStore* GetFontStore() const noexcept { return &this->fontStore; }
    TaskQueue* GetTaskQueue() const noexcept { return &this->taskQueue; }
    Executor* GetExecutor() const noexcept { return &this->executor; }
    Resources* GetResources() const noexcept { return &this->resources; }

    const std::string& GetResourcePath() const noexcept { return this->resourceDomainPath; }

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    Napi::Value GetResourceDomainPath(const Napi::CallbackInfo& info);
    void SetResourceDomainPath(const Napi::CallbackInfo& info, const Napi::Value& value);
    void Destroy(const Napi::CallbackInfo& info);

 private:
    mutable FontStore fontStore;
    mutable TaskQueue taskQueue;
    mutable Executor executor;
    mutable Resources resources;
    std::string resourceDomainPath;

    friend Napi::SafeObjectWrap<Stage>;
};

} // namespace ls
