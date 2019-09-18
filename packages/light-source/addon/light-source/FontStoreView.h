/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace ls {

class Stage;
class FontStore;

class FontStoreView : public Napi::ObjectWrap<FontStoreView> {
 public:
    explicit FontStoreView(const Napi::CallbackInfo& info);
    virtual ~FontStoreView();

    static Napi::Function Constructor(Napi::Env env);
    void Add(const Napi::CallbackInfo& info);
    void Remove(const Napi::CallbackInfo& info);
    Napi::Value All(const Napi::CallbackInfo& info);

 private:
    Stage* stage{ nullptr };
};

} // namespace ls
