/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLStageAdapter.h"
#include "SDLSceneAdapter.h"

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Value;

#define CallbackInstanceAccessor(NAME) \
    InstanceAccessor(#NAME, &SDLStageAdapter::Get_##NAME, &SDLStageAdapter::Set_##NAME)

namespace ls {

SDLStageAdapter::SDLStageAdapter(const CallbackInfo& info) : ObjectWrap<SDLStageAdapter>(info) {
}

Function SDLStageAdapter::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "SDLStageAdapter", {
            InstanceMethod("createSceneAdapter", &SDLStageAdapter::CreateSceneAdapter),
            InstanceMethod("resetCallbacks", &SDLStageAdapter::ResetCallbacks),
            InstanceMethod("processEvents", &SDLStageAdapter::ProcessEvents),
            CallbackInstanceAccessor(onQuit),
            CallbackInstanceAccessor(onKeyboardKeyUp),
            CallbackInstanceAccessor(onKeyboardKeyDown),
            CallbackInstanceAccessor(onGamepadButtonDown),
            CallbackInstanceAccessor(onGamepadButtonUp),
            CallbackInstanceAccessor(onGamepadHatMotion),
            CallbackInstanceAccessor(onGamepadAxisMotion),
            CallbackInstanceAccessor(onGamepadConnected),
            CallbackInstanceAccessor(onGamepadDisconnected),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value SDLStageAdapter::CreateSceneAdapter(const CallbackInfo& info) {
    return SDLSceneAdapter::Constructor(info.Env()).New({ info[0] });
}

Value SDLStageAdapter::ProcessEvents(const CallbackInfo& info) {
    static SDL_Event eventBuffer[NUM_EVENTS_PER_FRAME];

    SDL_PumpEvents();

    auto env{ info.Env() };
    auto eventIndex{ 0 };
    auto eventCount{ SDL_PeepEvents(eventBuffer, NUM_EVENTS_PER_FRAME, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) };

    if (eventCount < 0) {
        return Boolean::New(env, false);
    }

    while (eventIndex < eventCount) {
        HandleScope scope(env);
        float value;
        const auto& event = eventBuffer[eventIndex++];

        // TODO: update keyboard and gamepad states?

        switch (event.type) {
            case SDL_QUIT:
                if (!IsCallbackEmpty(onQuit)) {
                    Call(onQuit, {});
                }
                // TODO: user override?
                return Boolean::New(env, false);
            case SDL_KEYUP:
//              keyboard.keys[value] = 0
                if (!IsCallbackEmpty(onKeyboardKeyUp)) {
                    Call(onKeyboardKeyUp, { Number::New(env, event.key.keysym.scancode) });
                }
                break;
            case SDL_KEYDOWN:
//              keyboard.keys[value] = 1
                if (!IsCallbackEmpty(onKeyboardKeyDown)) {
                    Call(onKeyboardKeyDown, {
                        Number::New(env, event.key.keysym.scancode),
                        Number::New(env, event.key.repeat != 0),
                    });
                }
                break;
            case SDL_JOYBUTTONUP:
//              device.buttons[value] = 0
                if (!IsCallbackEmpty(onGamepadButtonUp)) {
                    Call(onGamepadButtonUp, {
                        Number::New(env, event.jbutton.which),
                        Number::New(env, event.jbutton.button),
                    });
                }
                break;
            case SDL_JOYBUTTONDOWN:
//              device.buttons[value] = 1
                if (!IsCallbackEmpty(onGamepadButtonDown)) {
                    Call(onGamepadButtonDown, {
                        Number::New(env, event.jbutton.which),
                        Number::New(env, event.jbutton.button),
                    });
                }
                break;
            case SDL_JOYHATMOTION:
                if (!IsCallbackEmpty(onGamepadHatMotion)) {
                    Call(onGamepadHatMotion, {
                        Number::New(env, event.jhat.which),
                        Number::New(env, event.jhat.hat),
                        Number::New(env, event.jhat.value),
                    });
                }
                break;
            case SDL_JOYAXISMOTION:
                value = static_cast<float>(event.jaxis.value);

                if (!IsCallbackEmpty(onGamepadAxisMotion)) {
                    Call(onGamepadAxisMotion, {
                        Number::New(env, event.jaxis.which),
                        Number::New(env, event.jaxis.axis),
                        Number::New(env, value < 0 ? -(value / static_cast<float>(SDL_JOYSTICK_AXIS_MIN))
                            : value / static_cast<float>(SDL_JOYSTICK_AXIS_MAX)),
                    });
                }
                break;
            case SDL_JOYDEVICEADDED:
                if (!IsCallbackEmpty(onGamepadConnected)) {
                    Call(onGamepadConnected, { Number::New(env, event.jdevice.which) });
                }
                break;
            case SDL_JOYDEVICEREMOVED:
                if (!IsCallbackEmpty(onGamepadDisconnected)) {
                    Call(onGamepadDisconnected, { Number::New(env, event.jdevice.which) });
                }
                break;
            default:
                break;
        }
    }

    return Boolean::New(env, true);
}

void SDLStageAdapter::ResetCallbacks(const CallbackInfo& info) {
    for (int i = 0; i < StageCallbacksCount; i++) {
        this->callbacks[i].Reset();
    }
}

void SDLStageAdapter::SetCallback(FunctionReference* function, const Napi::Value& value) {
    if (value.IsFunction()) {
        function->Reset(value.As<Function>(), 1);
    } else {
        function->Reset();
    }

    // TODO: throw if not null or undefined?
}

inline
void SDLStageAdapter::Call(const StageCallbacks callbackId, const std::initializer_list<napi_value>& args) {
    try {
        this->callbacks[callbackId](args);
    } catch (Napi::Error& error) {
        // TODO: print error!
        // fmt::println("SDLStageAdapter: Uncaught exception calling JS event handler: {}", error.what());
    }
}

inline
bool SDLStageAdapter::IsCallbackEmpty(const StageCallbacks callbackId) {
    return this->callbacks[callbackId].IsEmpty();
}

} // namespace ls

#undef CallbackInstanceAccessor
