/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLMixerAudioAdapter.h"
#include <algorithm>
#include <std17/algorithm>
#include <ls/BaseAudioSource.h>
#include <ls/BaseAudioDestination.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <ls/Timer.h>
#include <ls/Log.h>
#include <ls/Format.h>

using Napi::Boolean;
using Napi::Buffer;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::SafeObjectWrap;
using Napi::Value;

namespace ls {

SDL_RWops* LoadRW(Napi::Env env, const Napi::Value& value);
void FillVector(std::vector<std::string>* destination, const std::function<const char*(int32_t)>& get,
    const int32_t len) noexcept;
float ConstrainVolume(float volume) noexcept;
int32_t GetFadeOutMs(const CallbackInfo& info);
int32_t GetFadeInMs(const CallbackInfo& info);
int32_t GetLoops(const CallbackInfo& info);

constexpr auto MIX_MAX_VOLUME_F{ static_cast<float>(MIX_MAX_VOLUME) };

class SDLMixerSampleAudioSource : public SafeObjectWrap<SDLMixerSampleAudioSource>, public BaseAudioSource {
 public:
    explicit SDLMixerSampleAudioSource(const CallbackInfo& info) : SafeObjectWrap<SDLMixerSampleAudioSource>(info) {
    }

    virtual ~SDLMixerSampleAudioSource() = default;

    static Function GetClass(Napi::Env env) {
        return GetClassInternal<SDLMixerSampleAudioSource>(env, "SDLMixerSampleAudioSource");
    }

    void Load(const Napi::CallbackInfo& info) override {
        this->Destroy();

        auto env{ info.Env() };

        Timer t("sample: load");

        this->chunk = Mix_LoadWAV_RW(LoadRW(env, info[0]), SDL_TRUE);

        if (!this->chunk) {
            throw Error::New(env, "Load() failed to load audio data.");
        }
    }

    void Destroy(const Napi::CallbackInfo& info) override {
        this->Destroy();
    }

    Napi::Value GetVolume(const Napi::CallbackInfo& info) override {
        if (this->chunk) {
            return Number::New(info.Env(),
                ConstrainVolume(static_cast<float>(Mix_VolumeChunk(this->chunk, -1)) / MIX_MAX_VOLUME_F));
        }

        return Number::New(info.Env(), 0);
    }

    void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) override {
        if (this->chunk) {
            const auto volume{ ConstrainVolume(info[0].As<Number>().FloatValue()) };

            Mix_VolumeChunk(this->chunk, static_cast<int32_t>(volume * MIX_MAX_VOLUME_F));
        }
    }

    Napi::Value HasCapability(const Napi::CallbackInfo& info) override {
        switch (info[0].As<Number>().Int32Value()) {
            case AudioSourceCapabilityFadeIn:
            case AudioSourceCapabilityVolume:
            case AudioSourceCapabilityLoop:
                return Boolean::New(info.Env(), true);
            default:
                return Boolean::New(info.Env(), false);
        }
    }

    void Play(const Napi::CallbackInfo& info) override {
        if (this->chunk) {
            int32_t result;
            auto fadeInMs{ GetFadeInMs(info) };

            if (fadeInMs > 0) {
                result = Mix_FadeInChannel(-1, this->chunk, GetLoops(info) - 1, fadeInMs);
            } else {
                result = Mix_PlayChannel(-1, this->chunk, GetLoops(info) - 1);
            }

            if (result < 0) {
                LOG_ERROR(Mix_GetError());
            }
        }
    }

 private:
    void Destroy() {
        if (this->chunk) {
            Mix_FreeChunk(this->chunk);
            this->chunk = nullptr;
        }
    }

 private:
    Mix_Chunk* chunk{nullptr};

    friend SafeObjectWrap<SDLMixerSampleAudioSource>;
    friend BaseAudioSource;
};

class SDLMixerSampleAudioDestination
    : public SafeObjectWrap<SDLMixerSampleAudioDestination>, public BaseAudioDestination {
 public:
    explicit SDLMixerSampleAudioDestination(const CallbackInfo& info)
        : SafeObjectWrap<SDLMixerSampleAudioDestination>(info) {
    }

    virtual ~SDLMixerSampleAudioDestination() = default;

    static Function GetClass(Napi::Env env) {
        return GetClassInternal<SDLMixerSampleAudioDestination>(env, "SDLMixerSampleAudioDestination");
    }

    void Constructor(const Napi::CallbackInfo& info) override {
        FillVector(&this->decoders, &Mix_GetChunkDecoder, Mix_GetNumChunkDecoders());
    }

    Napi::Value CreateAudioSource(const CallbackInfo& info) override {
        auto env{ info.Env() };
        EscapableHandleScope scope(env);

        return scope.Escape(SDLMixerSampleAudioSource::GetClass(env).New({}));
    }

    Napi::Value GetVolume(const CallbackInfo& info) override {
        return Number::New(info.Env(),
            ConstrainVolume(static_cast<float>(Mix_Volume(-1, -1)) / MIX_MAX_VOLUME_F));
    }

    void SetVolume(const CallbackInfo& info, const Napi::Value& value) override {
        auto volume{ ConstrainVolume(info[0].As<Number>().FloatValue()) };

        Mix_Volume(-1, static_cast<int32_t>(volume * MIX_MAX_VOLUME_F));
    }

    void Pause(const CallbackInfo& info) override {
        Mix_Pause(-1);
    }

    void Resume(const CallbackInfo& info) override {
        Mix_Resume(-1);
    }

    void Stop(const CallbackInfo& info) override {
        auto fadeOutMs{ GetFadeOutMs(info) };

        if (fadeOutMs > 0) {
            Mix_FadeOutChannel(-1, fadeOutMs);
        } else {
            Mix_HaltChannel(-1);
        }
    }

    Napi::Value HasCapability(const CallbackInfo& info) override {
        // all capabilities supported
        return Boolean::New(info.Env(), true);
    }

    friend SafeObjectWrap<SDLMixerSampleAudioDestination>;
    friend BaseAudioDestination;
};

class SDLMixerStreamAudioSource : public SafeObjectWrap<SDLMixerStreamAudioSource>, public BaseAudioSource {
 public:
    explicit SDLMixerStreamAudioSource(const CallbackInfo& info) : SafeObjectWrap<SDLMixerStreamAudioSource>(info) {
    }

    virtual ~SDLMixerStreamAudioSource() = default;

    static Function GetClass(Napi::Env env) {
        return GetClassInternal<SDLMixerStreamAudioSource>(env, "SDLMixerStreamAudioSource");
    }

    void Load(const CallbackInfo& info) override {
        this->Destroy();

        auto env{ info.Env() };

        Timer t("music: load mus");

        this->music = Mix_LoadMUS_RW(LoadRW(env, info[0]), SDL_TRUE);

        if (!this->music) {
            throw Error::New(env, "Load() failed to load audio data.");
        }
    }

    void Destroy(const CallbackInfo& info) override {
        this->Destroy();
    }

    Napi::Value HasCapability(const CallbackInfo& info) override {
        switch (info[0].As<Number>().Int32Value()) {
            case AudioSourceCapabilityFadeIn:
            case AudioSourceCapabilityLoop:
                return Boolean::New(info.Env(), true);
            default:
                return Boolean::New(info.Env(), false);
        }
    }

    void Play(const CallbackInfo& info) override {
        if (this->music) {
            const auto fadeInMs{ GetFadeInMs(info) };
            int32_t result;

            if (fadeInMs > 0) {
                result = Mix_FadeInMusic(this->music, GetLoops(info) - 1, fadeInMs);
            } else {
                result = Mix_PlayMusic(this->music, GetLoops(info) - 1);
            }

            if (result < 0) {
                LOG_ERROR(Mix_GetError());
            }
        }
    }

 private:
    void Destroy() {
        if (this->music) {
            Mix_FreeMusic(this->music);
            this->music = nullptr;
        }
    }

 private:
    Mix_Music* music{nullptr};

    friend SafeObjectWrap<SDLMixerStreamAudioSource>;
    friend BaseAudioSource;
};

class SDLMixerStreamAudioDestination
    : public SafeObjectWrap<SDLMixerStreamAudioDestination>, public BaseAudioDestination {
 public:
    explicit SDLMixerStreamAudioDestination(const CallbackInfo& info)
            : SafeObjectWrap<SDLMixerStreamAudioDestination>(info) {
    }

    virtual ~SDLMixerStreamAudioDestination() = default;


    static Function GetClass(Napi::Env env) {
        return GetClassInternal<SDLMixerStreamAudioDestination>(env, "SDLMixerStreamAudioDestination");
    }

    void Constructor(const Napi::CallbackInfo& info) override {
        FillVector(&this->decoders, &Mix_GetMusicDecoder, Mix_GetNumMusicDecoders());
    }

    Napi::Value CreateAudioSource(const CallbackInfo& info) override {
        auto env{ info.Env() };
        EscapableHandleScope scope(env);

        return scope.Escape(SDLMixerStreamAudioSource::GetClass(env).New({}));
    }

    Napi::Value GetVolume(const CallbackInfo& info) override {
        return Number::New(info.Env(),
            ConstrainVolume(static_cast<float>(Mix_VolumeMusic(-1)) / MIX_MAX_VOLUME_F));
    }

    void SetVolume(const CallbackInfo& info, const Napi::Value& value) override {
        auto volume{ ConstrainVolume(info[0].As<Number>().FloatValue()) };

        Mix_VolumeMusic(static_cast<int32_t>(volume * MIX_MAX_VOLUME_F));
    }

    void Pause(const CallbackInfo& info) override {
        Mix_PauseMusic();
    }

    void Resume(const CallbackInfo& info) override {
        Mix_ResumeMusic();
    }

    void Stop(const CallbackInfo& info) override {
        auto fadeOutMs{ GetFadeOutMs(info) };

        if (fadeOutMs > 0) {
            Mix_FadeOutMusic(fadeOutMs);
        } else {
            Mix_HaltMusic();
        }
    }

    Napi::Value HasCapability(const CallbackInfo& info) override {
        // all capabilities supported
        return Boolean::New(info.Env(), true);
    }

    friend SafeObjectWrap<SDLMixerStreamAudioDestination>;
    friend BaseAudioDestination;
};

SDLMixerAudioAdapter::SDLMixerAudioAdapter(const CallbackInfo& info) : SafeObjectWrap(info) {
}

Function SDLMixerAudioAdapter::GetClass(Napi::Env env) {
    return GetClassInternal<SDLMixerAudioAdapter>(env, "SDLMixerAudioAdapter");
}

void SDLMixerAudioAdapter::Attach(const CallbackInfo& info) {
    if (this->isAttached) {
        return;
    }

    auto env{ info.Env() };
    Timer t("mixer init");

    if (SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        throw Error::New(env, Format("Failed to init SDL audio. SDL Error: %s", SDL_GetError()));
    }

    t.Log();

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 512) < 0) {
        throw Error::New(env, Format("Cannot open mixer. Error: %s", Mix_GetError()));
    }

    t.Log();

    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG
        // TODO: MIX_INIT_OPUS and MIX_INIT_MID not available in 2.0.0 headers.. ok to patch it in
        | /*MIX_INIT_MID*/ 0x00000020 | /*MIX_INIT_OPUS*/ 0x00000040);

    FillVector(
        &this->audioDevices,
        [](int32_t i) -> const char* {
            return SDL_GetAudioDeviceName(i, 0);
        },
        SDL_GetNumAudioDevices(0));

    this->isAttached = true;
}

void SDLMixerAudioAdapter::Detach(const CallbackInfo& info) {
    if (!this->isAttached) {
        return;
    }

    Mix_CloseAudio();

    // Documentation recommended way to call Mix_Quit()
    while (Mix_Init(0)) {
        Mix_Quit();
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    this->isAttached = false;
}

Value SDLMixerAudioAdapter::CreateSampleAudioDestination(const CallbackInfo& info) {
    auto env{ info.Env() };

    if (!this->isAttached) {
        throw Error::New(env, "SDLMixerAudioAdapter is not attached!");
    }

    EscapableHandleScope scope(env);

    return scope.Escape(SDLMixerSampleAudioDestination::GetClass(env).New({}));
}

Value SDLMixerAudioAdapter::CreateStreamAudioDestination(const CallbackInfo& info) {
    auto env{ info.Env() };

    if (!this->isAttached) {
        throw Error::New(env, "SDLMixerAudioAdapter is not attached!");
    }

    EscapableHandleScope scope(env);

    return scope.Escape(SDLMixerStreamAudioDestination::GetClass(env).New({}));
}

SDL_RWops* LoadRW(Napi::Env env, const Napi::Value& value) {
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

    return src;
}

void FillVector(std::vector<std::string>* destination, const std::function<const char*(int32_t)>& get,
        const int32_t len) noexcept {
    destination->clear();
    destination->reserve(len);

    for (int32_t i{0}; i < len; i++) {
        const auto str{ get(i) };

        destination->emplace_back(str ? str : "");
    }
}

float ConstrainVolume(const float volume) noexcept {
    return std17::clamp(volume, 0.f, 1.f);
}

int32_t GetPropertyAsInt(const CallbackInfo& info, const char* property, const int32_t defaultValue) {
    if (info.Length() > 0 && info[0].IsObject()) {
        HandleScope scope(info.Env());
        auto opts{ info[0].As<Object>() };
        auto fadeOutMs{ opts.Get(property) };

        if (fadeOutMs.IsNumber()) {
            return fadeOutMs.As<Number>().Int32Value();
        }
    }

    return defaultValue;
}

int32_t GetFadeOutMs(const CallbackInfo& info) {
    return GetPropertyAsInt(info, "fadeOutMs", 0);
}

int32_t GetFadeInMs(const CallbackInfo& info) {
    return GetPropertyAsInt(info, "fadeInMs", 0);
}

int32_t GetLoops(const CallbackInfo& info) {
    return GetPropertyAsInt(info, "loops", 1);
}

} // namespace ls