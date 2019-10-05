/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "AsyncTaskQueue.h"

namespace ls {

class FontStore;
class StageAdapter;

class Stage : public Napi::ObjectWrap<Stage> {
 public:
    explicit Stage(const Napi::CallbackInfo& info);
    virtual ~Stage();

    // javascript methods

    static Napi::Function Constructor(Napi::Env env);
    Napi::Value GetFontStoreView(const Napi::CallbackInfo& info);
    Napi::Value GetStageAdapter(const Napi::CallbackInfo& info);
    void SetStageAdapter(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetResourcePath(const Napi::CallbackInfo& info);
    void SetResourcePath(const Napi::CallbackInfo& info, const Napi::Value& value);
    void ProcessEvents(const Napi::CallbackInfo& info);

    // native methods

    FontStore* GetFontStore() const { return this->fontStore.get(); }
    StageAdapter* GetStageAdapter() const { return this->stageAdapter; }
    AsyncTaskQueue* GetAsyncTaskQueue() const { return &this->asyncTaskQueue; }
    const std::string& GetResourcePath() const { return this->resourcePath; }

 private:
    StageAdapter* stageAdapter{};
    std::unique_ptr<FontStore> fontStore;
    mutable AsyncTaskQueue asyncTaskQueue;
    std::string resourcePath;
};

} // namespace ls
