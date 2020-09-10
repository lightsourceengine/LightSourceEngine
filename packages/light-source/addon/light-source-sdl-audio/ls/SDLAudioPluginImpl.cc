/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/SDLAudioPluginImpl.h>

#include <ls/AudioDestination.h>
#include <ls/AudioSource.h>
#include <ls/string-ext.h>
#include <ls/SDL2.h>

using Napi::Boolean;
using Napi::Buffer;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::Number;
using Napi::Object;
using Napi::Value;

namespace ls {

class SDLAudioSourceImpl final : public AudioSourceInterface {
 public:
    explicit SDLAudioSourceImpl(const CallbackInfo& info) {
        this->deviceId = info[1].As<Number>().Int32Value();
    }
    virtual ~SDLAudioSourceImpl() = default;

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
            src = SDL2::SDL_RWFromMem(data, len);
        }

        if (!src) {
            throw Error::New(env, "Load() failed to read audio file from Buffer.");
        }

        SDL_AudioSpec spec{};
        auto chunk{ SDL2::SDL_LoadWAV_RW(src, SDL_TRUE, &spec, &this->audioBuffer, &this->audioBufferLen) };

        if (!chunk) {
            throw Error::New(env, "Load() failed to load audio data.");
        }
    }

    void Destroy(const CallbackInfo& info) override {
        this->Destroy();
    }

    void Play(const CallbackInfo& info) override {
        if (this->audioBuffer) {
            SDL2::SDL_ClearQueuedAudio(this->deviceId);
            SDL2::SDL_QueueAudio(this->deviceId, this->audioBuffer, this->audioBufferLen);
        }
    }

    Value GetVolume(const CallbackInfo& info) override {
        return Number::New(info.Env(), 0);
    }

    void SetVolume(const CallbackInfo& info, const Value& value) override {
    }

    Value HasCapability(const CallbackInfo& info) override {
        return Boolean::New(info.Env(), false);
    }

    void Finalize() override {
        delete this;
    }

 private:
    void Destroy() {
        if (this->audioBuffer) {
            SDL2::SDL_FreeWAV(this->audioBuffer);
            this->audioBuffer = nullptr;
            this->audioBufferLen = 0;
        }
    }

 private:
    uint8_t* audioBuffer{nullptr};
    uint32_t audioBufferLen{0};
    int32_t deviceId{-1};
};

class SDLAudioSampleAudioDestinationImpl final : public AudioDestinationInterface {
 public:
    explicit SDLAudioSampleAudioDestinationImpl(const CallbackInfo& info) {
        this->deviceId = info[1].As<Number>().Int32Value();
    }
    virtual ~SDLAudioSampleAudioDestinationImpl() = default;

    Napi::Value CreateAudioSource(const CallbackInfo& info) override {
        auto env{ info.Env() };
        EscapableHandleScope scope(env);

        return scope.Escape(
            AudioSource::Create<SDLAudioSourceImpl>(env, { Number::New(env, this->deviceId) }));
    }

    void Pause(const CallbackInfo& info) override {
        SDL2::SDL_PauseAudio(0);
    }

    void Resume(const CallbackInfo& info) override {
        SDL2::SDL_PauseAudio(1);
    }

    void Stop(const CallbackInfo& info) override {
        SDL2::SDL_ClearQueuedAudio(this->deviceId);
    }

    void Destroy(const CallbackInfo& info) override {
    }

    Value GetDecoders(const CallbackInfo& info) override {
        return Napi::NewStringArray(info.Env(), this->decoders);
    }

    Value GetVolume(const CallbackInfo& info) override {
        return Number::New(info.Env(), 0);
    }

    void SetVolume(const CallbackInfo& info, const Value& value) override {
    }

    void Finalize() override {
        delete this;
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
    std::vector<std::string> decoders{ "WAVE" };
    int32_t deviceId{-1};
};

SDLAudioPluginImpl::SDLAudioPluginImpl(const CallbackInfo& info) {
}

Value SDLAudioPluginImpl::IsAttached(const CallbackInfo& info) {
    return Boolean::New(info.Env(), this->isAttached);
}

Value SDLAudioPluginImpl::GetAudioDevices(const CallbackInfo& info) {
    return NewStringArray(info.Env(), this->audioDevices);
}

void SDLAudioPluginImpl::Destroy(const Napi::CallbackInfo& info) {
    this->Detach(info);
}

void SDLAudioPluginImpl::Attach(const CallbackInfo& info) {
    if (this->isAttached) {
        return;
    }

    auto env{ info.Env() };

    // Initialize SDL Audio.
    if (SDL2::SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL2::SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        throw Error::New(env, Format("Failed to init SDL audio. SDL Error: %s", SDL2::SDL_GetError()));
    }

    // Open an audio device.
    SDL_AudioSpec desired{};

    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 512;

    if (SDL2::SDL_OpenAudio(&desired, nullptr) != 0) {
        throw Error::New(env, Format("Cannot open audio. SDL_OpenAudio: %s", SDL2::SDL_GetError()));
    }

    // Reserved device id for SDL_OpenAudio is 1.
    this->deviceId = 1;

    // Ensure the device is silent.
    SDL2::SDL_PauseAudio(0);

    auto len{ SDL2::SDL_GetNumAudioDevices(0) };

    // Get list of audio device names.
    this->audioDevices.clear();

    for (auto i = 0; i < len; i++) {
        auto str{ SDL2::SDL_GetAudioDeviceName(i, 0) };

        this->audioDevices.emplace_back(str ? str : "");
    }

    this->isAttached = true;
}

void SDLAudioPluginImpl::Detach(const CallbackInfo& info) {
    if (!this->isAttached) {
        return;
    }

    SDL2::SDL_CloseAudio();
    SDL2::SDL_QuitSubSystem(SDL_INIT_AUDIO);

    this->isAttached = false;
}

Value SDLAudioPluginImpl::CreateSampleAudioDestination(const CallbackInfo& info) {
    auto env{ info.Env() };

    if (!this->isAttached) {
        throw Error::New(env, "SDLAudioPlugin is not attached!");
    }

    EscapableHandleScope scope(env);

    auto destination{ AudioDestination::Create<SDLAudioSampleAudioDestinationImpl>(
        env, { Number::New(env, this->deviceId) }) };

    return scope.Escape(destination);
}

Value SDLAudioPluginImpl::CreateStreamAudioDestination(const CallbackInfo& info) {
    return info.Env().Null();
}

void SDLAudioPluginImpl::Finalize() {
    delete this;
}

} // namespace ls
