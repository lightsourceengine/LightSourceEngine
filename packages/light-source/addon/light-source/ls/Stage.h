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

namespace ls {

class StageAdapter;

class Stage : public Napi::SafeObjectWrap<Stage> {
 public:
    explicit Stage(const Napi::CallbackInfo& info);
    virtual ~Stage() = default;

    FontStore* GetFontStore() const noexcept { return &this->fontStore; }
    StageAdapter* GetStageAdapter() const noexcept { return this->stageAdapter; }
    TaskQueue* GetTaskQueue() const noexcept { return &this->taskQueue; }
    Executor* GetExecutor() const noexcept { return &this->executor; }
    const std::string& GetResourcePath() const noexcept { return this->resourcePath; }
    void Destroy() noexcept;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    Napi::Value GetStageAdapter(const Napi::CallbackInfo& info);
    void SetStageAdapter(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetResourcePath(const Napi::CallbackInfo& info);
    void SetResourcePath(const Napi::CallbackInfo& info, const Napi::Value& value);
    void ProcessEvents(const Napi::CallbackInfo& info);

 private:
    StageAdapter* stageAdapter{};
    mutable FontStore fontStore;
    mutable TaskQueue taskQueue;
    mutable Executor executor;
    std::string resourcePath;

    friend Napi::SafeObjectWrap<Stage>;
};

} // namespace ls
