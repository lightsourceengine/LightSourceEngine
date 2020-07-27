/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/BaseAudioAdapter.h>

namespace ls {

class RefAudioAdapter : public Napi::SafeObjectWrap<RefAudioAdapter>, public BaseAudioAdapter {
 public:
    explicit RefAudioAdapter(const Napi::CallbackInfo& info);
    virtual ~RefAudioAdapter() = default;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;

    Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;
    Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) override;

    friend Napi::SafeObjectWrap<RefAudioAdapter>;
    friend BaseAudioAdapter;
};

} // namespace ls
