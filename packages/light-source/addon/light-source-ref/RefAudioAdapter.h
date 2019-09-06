/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <BaseAudioAdapter.h>

namespace ls {

class RefAudioAdapter : public Napi::ObjectWrap<RefAudioAdapter>, public BaseAudioAdapter {
 public:
    explicit RefAudioAdapter(const Napi::CallbackInfo& info);
    virtual ~RefAudioAdapter() = default;

    static Napi::Function Constructor(Napi::Env env);

    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;

    Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;
    Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) override;
};

} // namespace ls
