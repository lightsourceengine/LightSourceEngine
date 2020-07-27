/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefAudioAdapter.h"
#include <ls/BaseAudioSource.h>
#include <ls/BaseAudioDestination.h>

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::HandleScope;
using Napi::SafeObjectWrap;
using Napi::Value;

namespace ls {

class RefAudioSource : public SafeObjectWrap<RefAudioSource>, public BaseAudioSource {
 public:
    explicit RefAudioSource(const CallbackInfo& info) : SafeObjectWrap<RefAudioSource>(info) {
    }

    virtual ~RefAudioSource() = default;

    static Function GetClass(Napi::Env env) {
        return GetClassInternal<RefAudioSource>(env, "RefAudioSource");
    }

    void Load(const CallbackInfo& info) override {
    }

    void Destroy(const CallbackInfo& info) override {
    }

    Napi::Value HasCapability(const CallbackInfo& info) override {
        return Boolean::New(info.Env(), true);
    }

    friend SafeObjectWrap<RefAudioSource>;
    friend BaseAudioSource;
};

class RefAudioDestination : public SafeObjectWrap<RefAudioDestination>, public BaseAudioDestination {
 public:
    explicit RefAudioDestination(const CallbackInfo& info) : SafeObjectWrap<RefAudioDestination>(info) {
    }

    virtual ~RefAudioDestination() = default;

    static Function GetClass(Napi::Env env) {
        return GetClassInternal<RefAudioDestination>(env, "RefAudioDestination");
    }

    void Constructor(const CallbackInfo& info) override {
        this->decoders = { "WAVE" };
    }

    Napi::Value CreateAudioSource(const CallbackInfo& info) override {
        auto env{ info.Env() };
        EscapableHandleScope scope(env);

        return scope.Escape(RefAudioSource::GetClass(env).New({}));
    }

    Napi::Value HasCapability(const CallbackInfo& info) override {
        return Boolean::New(info.Env(), true);
    }

    friend SafeObjectWrap<RefAudioDestination>;
    friend BaseAudioDestination;
};

RefAudioAdapter::RefAudioAdapter(const CallbackInfo& info) : SafeObjectWrap(info) {
}

Function RefAudioAdapter::GetClass(Napi::Env env) {
    return GetClassInternal<RefAudioAdapter>(env, "RefAudioAdapter");
}

void RefAudioAdapter::Attach(const Napi::CallbackInfo& info) {
    this->isAttached = true;
}

void RefAudioAdapter::Detach(const Napi::CallbackInfo& info) {
    this->isAttached = false;
}

Value RefAudioAdapter::CreateSampleAudioDestination(const CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);

    return scope.Escape(RefAudioDestination::GetClass(env).New({}));
}

Value RefAudioAdapter::CreateStreamAudioDestination(const CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);

    return scope.Escape(RefAudioDestination::GetClass(env).New({}));
}

void RefAudioAdapter::Constructor(const CallbackInfo &info) {
    this->audioDevices = { "Reference" };
}

} // namespace ls
