/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLPlatformPluginExports.h"

#include <lse/SDLPlatformPlugin.h>
#include <ObjectBuilder.h>
#include <lse/Config.h>
#include <napi-ext.h>
#include <lse/SDLGraphicsContextImpl.h>
#include <lse/Log.h>

using Napi::Boolean;
using Napi::Number;

namespace lse {
namespace bindings {

enum PluginCallback {
  OnKeyboardScanCode,
  OnGamepadStatus,
  OnGamepadAxis,
  OnGamepadHat,
  OnGamepadButton,
  OnGamepadButtonMapped,
  OnGamepadAxisMapped,
  OnQuit,
  PluginCallbackCount
};

static SDLPlatformPlugin* sPlugin{};
static Napi::FunctionReference sPluginCallbacks[PluginCallbackCount]{};

static void ResetCallbacks() noexcept;

static napi_value Attach(napi_env env, napi_callback_info info) {
  NAPI_TRY(env, sPlugin->Attach());
  return nullptr;
}

static napi_value Detach(napi_env env, napi_callback_info info) noexcept {
  ResetCallbacks();
  sPlugin->Detach();
  return nullptr;
}

static napi_value IsAttached(napi_env env, napi_callback_info info) {
  return Napi::Boolean::New(env, sPlugin->IsAttached());
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  ResetCallbacks();
  sPlugin->Destroy();
  return nullptr;
}

static napi_value GetDisplays(napi_env env, napi_callback_info info) {
  Napi::ObjectBuilder builder(env);
  auto result{ Napi::Array::New(env) };

  auto NewDisplayMode = [](napi_env env, const SDLDisplayMode& displayMode) {
    return Napi::ObjectBuilder(env)
      .WithValue("width", displayMode.width)
      .WithValue("height", displayMode.height)
      .Freeze()
      .ToObject();
  };

  for (const auto& display : sPlugin->GetDisplays()) {
    auto modes{ Napi::Array::New(env) };

    for (const auto& mode : display.modes) {
      modes.Set(modes.Length(), NewDisplayMode(env, mode));
    }

    result.Set(result.Length(), builder
        .WithValue("id", display.id)
        .WithValue("name", display.name.c_str())
        .WithValue("defaultMode", NewDisplayMode(env, display.defaultMode))
        .WithValue("modes", modes)
        .Freeze()
        .ToObject());
  }

  return result;
}

static napi_value GetVideoDriverNames(napi_env env, napi_callback_info info) {
  return Napi::ToArray<Napi::String>(env, sPlugin->GetVideoDriverNames());
}

static void ResetCallbacks() noexcept {
  for (auto& sPluginEventCallback : sPluginCallbacks) {
    sPluginEventCallback = Napi::FunctionReference();
  }
}

static napi_value ResetCallbacks(napi_env env, napi_callback_info info) noexcept {
  ResetCallbacks();
  return nullptr;
}

static napi_value CreateGraphicsContext(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci{ env, info };

  return GraphicsContext::Create<SDLGraphicsContextImpl>(env, ci[0]);
}

static napi_value GetScanCodeState(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci{ env, info };
  bool result = ci[0].IsNumber() && sPlugin->GetScanCodeState(ci[0].As<Napi::Number>().Int32Value());

  return Napi::Boolean::New(env, result);
}

static napi_value LoadGameControllerMappings(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci{ env, info };
  auto value{ ci[0] };
  int32_t count;

  if (value.IsString()) {
    auto valueLen{ Napi::StringByteLength(value) };

    if (valueLen < Napi::SizeOfCopyUtf8Buffer()) {
      count = sPlugin->LoadGameControllerMappings(Napi::CopyUtf8(value), valueLen);
    } else {
      std::string tempString = value.As<Napi::String>();
      count = sPlugin->LoadGameControllerMappings(tempString.c_str(), tempString.size());
    }
  } else if (value.IsBuffer()) {
    auto buffer{ value.As<Napi::Buffer<uint8_t>>() };

    count = sPlugin->LoadGameControllerMappings(buffer.Data(), buffer.Length());
  } else {
    count = 0;
  }

  return Napi::NewNumber(env, count);
}

static napi_value GetGameControllerMapping(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci{ env, info };
  auto mapping{ sPlugin->GetGameControllerMapping(Napi::CopyUtf8(ci[0])) };

  return Napi::String::New(env, mapping);
}

static napi_value ProcessEvents(napi_env env, napi_callback_info info) {
  return Napi::Boolean::New(env, sPlugin->ProcessEvents());
}

static napi_value GetGamepadInstanceIds(napi_env env, napi_callback_info info) {
  return Napi::ToArray<Napi::Number>(env, sPlugin->GetGamepadInstanceIds());
}

static napi_value IsKeyDown(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci(env, info);

  bool result = ci[0].IsNumber()
      && ci[1].IsNumber()
      && sPlugin->IsKeyDown(ci[0].As<Number>(), ci[1].As<Number>());

  return Napi::Boolean::New(env, result);
}

static napi_value GetAnalogValue(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci(env, info);

  if (ci[0].IsNumber() && ci[1].IsNumber()) {
    return Napi::NewNumber(env, sPlugin->GetAnalogValue(ci[0].As<Number>(), ci[1].As<Number>()));
  }

  return Napi::NewNumber(env, 0.f);
}

static napi_value GetButtonState(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci(env, info);

  bool result = ci[0].IsNumber()
      && ci[1].IsNumber()
      && sPlugin->GetButtonState(ci[0].As<Number>(), ci[1].As<Number>());

  return Napi::Boolean::New(env, result);
}

static napi_value GetAxisState(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci(env, info);

  if (ci[0].IsNumber() && ci[1].IsNumber()) {
    return Napi::NewNumber(env, sPlugin->GetAxisState(ci[0].As<Number>(), ci[1].As<Number>()));
  }

  return Napi::NewNumber(env, 0.f);
}

static napi_value GetHatState(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci(env, info);

  if (ci[0].IsNumber() && ci[1].IsNumber()) {
    return Napi::NewNumber(env, sPlugin->GetHatState(ci[0].As<Number>(), ci[1].As<Number>()));
  }

  return Napi::NewNumber(env, 0);
}

static napi_value GetGamepadInfo(napi_env env, napi_callback_info info) {
  Napi::CallbackInfo ci(env, info);
  int32_t instanceId;

  if (ci[0].IsNumber()) {
    instanceId = ci[0].As<Number>();
  } else {
    instanceId = -1;
  }

  auto gamepadInfo{ sPlugin->GetGamepadInfo(instanceId) };

  return Napi::ObjectBuilder(env)
    .WithValue("id", gamepadInfo.instanceId)
    .WithValue("type", "gamepad")
    .WithValue("name", gamepadInfo.name)
    .WithValue("buttonCount", gamepadInfo.buttonCount)
    .WithValue("hatCount", gamepadInfo.hatCount)
    .WithValue("axisCount", gamepadInfo.axisCount)
    .WithValue("uuid", gamepadInfo.uuid)
    .Freeze()
    .ToObject();
}

static napi_value MakeArg(const Napi::Env& env, int32_t value) {
  return Napi::NewNumber(env, value);
}

static napi_value MakeArg(const Napi::Env& env, bool value) {
  return Napi::Boolean::New(env, value);
}

static napi_value MakeArg(const Napi::Env& env, float value) {
  return Napi::NewNumber(env, value);
}

template<typename T, typename ... Args>
static void InvokeCallback(const char* name, T callbackType, Args ... args) noexcept {
  auto& callback{ sPluginCallbacks[callbackType] };

  if (!callback.IsEmpty()) {
    try {
      Napi::SafeHandleScope scope(callback.Env());
      callback({ MakeArg(callback.Env(), args)... });
    } catch (const std::exception& e) {
      auto LAMBDA_FUNCTION = name;
      LOG_ERROR_LAMBDA("Unhanded JS exception: %s", e.what());
    }
  }
}

static bool InvokeCallback(const char* name, PluginCallback callbackType, bool defaultReturn) noexcept {
  auto& callback{ sPluginCallbacks[callbackType] };

  if (!callback.IsEmpty()) {
    try {
      Napi::SafeHandleScope scope(callback.Env());
      auto result{ callback({}) };

      return result.ToBoolean();
    } catch (const std::exception& e) {
      auto LAMBDA_FUNCTION = name;
      LOG_ERROR_LAMBDA("Unhanded JS exception: %s", e.what());
    }
  }

  return defaultReturn;
}

static void Init(const Napi::Env& env) {
  if (sPlugin) {
    throw Napi::Error::New(env, "SDL platform plugin has already been loaded.");
  }

  NAPI_TRY(env, SDL2::Open());

  sPlugin = new SDLPlatformPlugin();

  sPlugin->onKeyboardScanCode = [](int32_t scanCode, bool pressed , bool repeat) noexcept {
    InvokeCallback("onKeyboardScanCode", OnKeyboardScanCode, scanCode, pressed, repeat);
  };

  sPlugin->onGamepadStatus = [](int32_t instanceId, bool connected) noexcept {
    InvokeCallback("onGamepadStatus", OnGamepadStatus, instanceId, connected);
  };

  sPlugin->onGamepadAxis = [](int32_t instanceId, int32_t axis, float value) noexcept {
    InvokeCallback("onGamepadAxis", OnGamepadAxis, instanceId, axis, value);
  };

  sPlugin->onGamepadAxisMapped = [](int32_t instanceId, int32_t axis, float value) noexcept {
    InvokeCallback("onGamepadAxisMapped", OnGamepadAxisMapped, instanceId, axis, value);
  };

  sPlugin->onGamepadHat = [](int32_t instanceId, int32_t hat, int32_t value) noexcept {
    InvokeCallback("onGamepadHat", OnGamepadHat, instanceId, hat, value);
  };

  sPlugin->onGamepadButton = [](int32_t instanceId, int32_t button, bool pressed) noexcept {
    InvokeCallback("onGamepadButton", OnGamepadButton, instanceId, button, pressed);
  };

  sPlugin->onGamepadButtonMapped = [](int32_t instanceId, int32_t button, bool pressed) noexcept {
    InvokeCallback("onGamepadButtonMapped", OnGamepadButtonMapped, instanceId, button, pressed);
  };

  sPlugin->onQuit = []() noexcept -> bool {
    InvokeCallback("onQuit", OnQuit, true);

    return true;
  };

  napi_add_env_cleanup_hook(env, [](void*) { ResetCallbacks(); }, nullptr);
}

Napi::Object SDLPlatformPluginExports(const Napi::Env& env) {
  Init(env);

#define WithPluginEventProperty(PROP, EVENT)                                \
  WithProperty(                                                             \
    PROP,                                                                   \
    [](napi_env, napi_callback_info) -> napi_value {                        \
      return sPluginCallbacks[EVENT].Value();                               \
    },                                                                      \
    [](napi_env env, napi_callback_info info) -> napi_value {               \
      Napi::CallbackInfo ci{ env, info };                                   \
      Napi::AssignFunctionReference(sPluginCallbacks[EVENT], ci[0]);        \
      return nullptr;                                                       \
    })

  return Napi::ObjectBuilder(env)
      .WithValue("id", kPluginPlatformSdl)
      .WithValue("type", "platform")
      .WithMethod("attach", &Attach)
      .WithMethod("detach", &Detach)
      .WithMethod("destroy", &Destroy)
      .WithMethod("isAttached", &IsAttached)

      .WithMethod("getDisplays", &GetDisplays)
      .WithMethod("getVideoDriverNames", &GetVideoDriverNames)

      .WithMethod("createGraphicsContext", &CreateGraphicsContext)
      .WithMethod("processEvents", &ProcessEvents)

      .WithMethod("getScanCodeState", &GetScanCodeState)

      .WithMethod("getGameControllerMapping", &GetGameControllerMapping)
      .WithMethod("getGamepadInstanceIds", &GetGamepadInstanceIds)
      .WithMethod("loadGameControllerMappings", &LoadGameControllerMappings)
      .WithMethod("isKeyDown", &IsKeyDown)
      .WithMethod("getAnalogValue", &GetAnalogValue)
      .WithMethod("getButtonState", &GetButtonState)
      .WithMethod("getAxisState", &GetAxisState)
      .WithMethod("getHatState", &GetHatState)
      .WithMethod("getGamepadInfo", &GetGamepadInfo)

      .WithPluginEventProperty("onKeyboardScanCode", OnKeyboardScanCode)
      .WithPluginEventProperty("onGamepadStatus", OnGamepadStatus)
      .WithPluginEventProperty("onGamepadAxis", OnGamepadAxis)
      .WithPluginEventProperty("onGamepadAxisMapped", OnGamepadAxisMapped)
      .WithPluginEventProperty("onGamepadHat", OnGamepadHat)
      .WithPluginEventProperty("onGamepadButton", OnGamepadButton)
      .WithPluginEventProperty("onGamepadButtonMapped", OnGamepadButtonMapped)
      .WithPluginEventProperty("onQuit", OnQuit)
      .WithMethod("resetCallbacks", &ResetCallbacks)

      .ToObject();

#undef WithPluginEventProperty
}

} // namespace bindings
} // namespace lse
