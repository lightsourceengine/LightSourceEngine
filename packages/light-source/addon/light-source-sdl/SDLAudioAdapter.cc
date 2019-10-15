/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLAudioAdapter.h"
#include <ls/BaseAudioSource.h>
#include <ls/BaseAudioDestination.h>
#include <napi-ext.h>
#include <ls/Format.h>

using Napi::Boolean;
using Napi::Buffer;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::Number;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::Value;

namespace ls {

class SDLAudioSource : public ObjectWrap<SDLAudioSource>, public BaseAudioSource {
 public:
    explicit SDLAudioSource(const CallbackInfo& info) : ObjectWrap<SDLAudioSource>(info) {
        this->deviceId = info[0].As<Number>().Int32Value();
    }

    virtual ~SDLAudioSource() = default;

    static Function Constructor(Napi::Env env) {
        struct SDLAudioSourceClassName {
            static const char* Get() {
                return "SDLAudioSource";
            }
        };

        return ConstructorInternal<SDLAudioSource, SDLAudioSourceClassName>(env);
    }

    void Load(const CallbackInfo& info) override {
        this->Destroy();

        auto env{ info.Env() };
        auto value{ info[0] };

        if (!value.IsBuffer()) {
            throw Error::New(env, "Load() expects a Buffer");
        }

        auto buffer{ value.As<Buffer<uint8_t>>() };
        auto data{ static_cast<void*>(buffer.Data()) };
        auto len{ static_cast<int32_t>(buffer.Length()) };
        SDL_RWops* src{ nullptr };

        if (data && len > 0) {
            src = SDL_RWFromMem(data, len);
        }

        if (!src) {
            throw Error::New(env, "Load() failed to read audio file from Buffer.");
        }

        SDL_AudioSpec spec{};
        auto chunk{ SDL_LoadWAV_RW(src, SDL_TRUE, &spec, &this->audioBuffer, &this->audioBufferLen) };

        if (!chunk) {
            throw Error::New(env, "Load() failed to load audio data.");
        }
    }

    void Destroy(const CallbackInfo& info) override {
        this->Destroy();
    }

    void Play(const CallbackInfo& info) override {
        if (this->audioBuffer) {
            SDL_ClearQueuedAudio(this->deviceId);
            SDL_QueueAudio(this->deviceId, this->audioBuffer, this->audioBufferLen);
        }
    }

 private:
    void Destroy() {
        if (this->audioBuffer) {
            SDL_FreeWAV(this->audioBuffer);
            this->audioBuffer = nullptr;
            this->audioBufferLen = 0;
        }
    }

 private:
    uint8_t* audioBuffer{nullptr};
    uint32_t audioBufferLen{0};
    int32_t deviceId{-1};
};

class SDLAudioSampleAudioDestination : public ObjectWrap<SDLAudioSampleAudioDestination>, public BaseAudioDestination {
 public:
    explicit SDLAudioSampleAudioDestination(const CallbackInfo& info)
            : ObjectWrap<SDLAudioSampleAudioDestination>(info) {
        this->deviceId = info[0].As<Number>().Int32Value();
        // Only the WAVE decoder is supported by SDL Audio.
        this->decoders = { "WAVE" };
    }

    virtual ~SDLAudioSampleAudioDestination() = default;

    static Function Constructor(Napi::Env env) {
        struct SDLAudioSampleAudioDestinationClassName {
            static const char* Get() {
                return "SDLAudioSampleAudioDestination";
            }
        };

        return ConstructorInternal<SDLAudioSampleAudioDestination, SDLAudioSampleAudioDestinationClassName>(env);
    }

    Napi::Value CreateAudioSource(const CallbackInfo& info) override {
        auto env{ info.Env() };
        EscapableHandleScope scope(env);

        return scope.Escape(SDLAudioSource::Constructor(env).New({}));
    }

    void Pause(const CallbackInfo& info) override {
        SDL_PauseAudio(0);
    }

    void Resume(const CallbackInfo& info) override {
        SDL_PauseAudio(1);
    }

    void Stop(const CallbackInfo& info) override {
        SDL_ClearQueuedAudio(this->deviceId);
    }

    Napi::Value HasCapability(const CallbackInfo& info) override {
        switch (info[0].As<Number>().Int32Value()) {
            case AudioDestinationCapabilityStop:
            case AudioDestinationCapabilityResume:
            case AudioDestinationCapabilityPause:
                return Boolean::New(info.Env(), true);
            case AudioDestinationCapabilityVolume:
            case AudioDestinationCapabilityFadeOut:
            default:
                return Boolean::New(info.Env(), false);
        }
    }

 private:
    int32_t deviceId{-1};
};

SDLAudioAdapter::SDLAudioAdapter(const CallbackInfo& info) : ObjectWrap(info) {
}

Function SDLAudioAdapter::Constructor(Napi::Env env) {
    struct SDLAudioAdapterClassName {
        static const char* Get() {
            return "SDLAudioAdapter";
        }
    };

    return ConstructorInternal<SDLAudioAdapter, SDLAudioAdapterClassName>(env);
}

void SDLAudioAdapter::Attach(const CallbackInfo& info) {
    if (this->isAttached) {
        return;
    }

    auto env{ info.Env() };

    // Initialize SDL Audio.
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        throw Error::New(env, Format("Failed to init SDL audio. SDL Error: %s", SDL_GetError()));
    }

    // Open an audio device.
    SDL_AudioSpec desired{};

    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 512;

    if (SDL_OpenAudio(&desired, nullptr) != 0) {
        throw Error::New(env, Format("Cannot open audio. SDL_OpenAudio: %s", SDL_GetError()));
    }

    // Reserved device id for SDL_OpenAudio is 1.
    this->deviceId = 1;

    // Ensure the device is silent.
    SDL_PauseAudio(0);

    auto len{ SDL_GetNumAudioDevices(0) };

    // Get list of audio device names.
    this->audioDevices.clear();

    for (auto i = 0; i < len; i++) {
        auto str{ SDL_GetAudioDeviceName(i, 0) };

        this->audioDevices.emplace_back(str ? str : "");
    }

    this->isAttached = true;
}

void SDLAudioAdapter::Detach(const CallbackInfo& info) {
    if (!this->isAttached) {
        return;
    }

    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    this->isAttached = false;
}

Value SDLAudioAdapter::CreateSampleAudioDestination(const CallbackInfo& info) {
    auto env{ info.Env() };

    if (!this->isAttached) {
        throw Error::New(env, "SDLAudioAdapter is not attached!");
    }

    EscapableHandleScope scope(env);

    return scope.Escape(SDLAudioSource::Constructor(env).New({ Number::New(env, this->deviceId) }));
}

} // namespace ls
