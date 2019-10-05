/*
 * Copyright (C) 2019 Daniel Anderson
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
using Napi::ObjectWrap;
using Napi::Value;

namespace ls {

class RefAudioSource : public ObjectWrap<RefAudioSource>, public BaseAudioSource {
 public:
    explicit RefAudioSource(const CallbackInfo& info) : ObjectWrap<RefAudioSource>(info) {
    }

    virtual ~RefAudioSource() = default;

    static Function Constructor(Napi::Env env) {
        struct RefAudioSourceClassName {
            static const char* Get() {
                return "RefAudioSource";
            }
        };

        return ConstructorInternal<RefAudioSource, RefAudioSourceClassName>(env);
    }

    void Load(const CallbackInfo& info) override {
    }

    void Destroy(const CallbackInfo& info) override {
    }

    Napi::Value HasCapability(const CallbackInfo& info) override {
        return Boolean::New(info.Env(), true);
    }
};

class RefAudioDestination : public ObjectWrap<RefAudioDestination>, public BaseAudioDestination {
 public:
    explicit RefAudioDestination(const CallbackInfo& info) : ObjectWrap<RefAudioDestination>(info) {
        this->decoders = { "WAVE" };
    }

    virtual ~RefAudioDestination() = default;

    static Function Constructor(Napi::Env env) {
        struct RefAudioDestinationClassName {
            static const char* Get() {
                return "RefAudioDestination";
            }
        };

        return ConstructorInternal<RefAudioDestination, RefAudioDestinationClassName>(env);
    }

    Napi::Value CreateAudioSource(const CallbackInfo& info) override {
        auto env{ info.Env() };
        EscapableHandleScope scope(env);

        return scope.Escape(RefAudioSource::Constructor(env).New({}));
    }

    Napi::Value HasCapability(const CallbackInfo& info) override {
        return Boolean::New(info.Env(), true);
    }
};

RefAudioAdapter::RefAudioAdapter(const CallbackInfo& info) : ObjectWrap(info) {
    this->audioDevices = { "Reference" };
}

Function RefAudioAdapter::Constructor(Napi::Env env) {
    struct RefAudioAdapterClassName {
        static const char* Get() {
            return "RefAudioAdapter";
        }
    };

    return ConstructorInternal<RefAudioAdapter, RefAudioAdapterClassName>(env);
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

    return scope.Escape(RefAudioDestination::Constructor(env).New({}));
}

Value RefAudioAdapter::CreateStreamAudioDestination(const CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);

    return scope.Escape(RefAudioDestination::Constructor(env).New({}));
}

} // namespace ls
