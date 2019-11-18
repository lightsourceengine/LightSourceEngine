/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>

namespace ls {

/**
 * Adapts the native audio system to the javascript AudioManager.
 */
class AudioAdapter : public virtual Napi::SafeObjectWrapBase {
 public:
    // javascript methods

    virtual void Attach(const Napi::CallbackInfo& info) = 0;
    virtual void Detach(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value IsAttached(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetAudioDevices(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) = 0;
};

/**
 * Audio output buffer.
 */
class AudioDestination {
 public:
    virtual ~AudioDestination() = default;

    // javascript methods

    virtual Napi::Value GetDecoders(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value CreateAudioSource(const Napi::CallbackInfo& info) = 0;
    virtual void Resume(const Napi::CallbackInfo& info) = 0;
    virtual void Pause(const Napi::CallbackInfo& info) = 0;
    virtual void Stop(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetVolume(const Napi::CallbackInfo& info) = 0;
    virtual void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) = 0;
    virtual Napi::Value HasCapability(const Napi::CallbackInfo& info) = 0;
};

/**
 * Audio source that can be rendered to a destination.
 */
class AudioSource {
 public:
    virtual ~AudioSource() = default;

    // javascript methods

    virtual void Load(const Napi::CallbackInfo& info) = 0;
    virtual void Destroy(const Napi::CallbackInfo& info) = 0;
    virtual void Play(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetVolume(const Napi::CallbackInfo& info) = 0;
    virtual void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) = 0;
    virtual Napi::Value HasCapability(const Napi::CallbackInfo& info) = 0;
};

} // namespace ls
