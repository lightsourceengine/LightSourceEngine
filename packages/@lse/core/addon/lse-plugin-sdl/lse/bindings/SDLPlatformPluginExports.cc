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
#include <lse/SDLGraphicsContext.h>
#include <lse/Log.h>
#include <lse/bindings/JSGraphicsContext.h>

using Napi::Boolean;
using Napi::Number;

/*
 * Dev Notes
 *
 * SDLPlatformPlugin does not use ObjectWrap for bindings. ObjectWrap bloats the code size due
 * to templates and wrappers to interop with C add runtime overhead. Since the plugin is a
 * singleton, I opted to use a static variable for the native SDLPlatform plugin instance. I
 * create an Object and add wrapped native functions that call the singleton. This is a bit
 * messy, but it works (won't work for multiple contexts).
 *
 * The object creation is through the C NAPI, to reduce overhead. However, NAPI exceptions are
 * enabled, so the native wrappers have to deal with exceptions. In the future, exceptions should
 * be turned off to simplify this code.
 */

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

static napi_value Attach(napi_env env, napi_callback_info info) noexcept {
  try {
    sPlugin->Attach();
  } catch (const std::exception& e) {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
  }
  return nullptr;
}

static napi_value Detach(napi_env env, napi_callback_info info) noexcept {
  ResetCallbacks();
  sPlugin->Detach();
  return nullptr;
}

static napi_value IsAttached(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return Napi::Boolean::New(env, sPlugin->IsAttached()))
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  ResetCallbacks();
  sPlugin->Destroy();
  return nullptr;
}

static napi_value GetDisplaysThrows(napi_env env) {
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

static napi_value GetDisplays(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return GetDisplaysThrows(env))
}

static napi_value GetVideoDriverNames(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return Napi::ToArray<Napi::String>(env, sPlugin->GetVideoDriverNames()))
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

static napi_value CreateGraphicsContext(napi_env env, napi_callback_info info) noexcept {
  try {
    Napi::CallbackInfo ci{ env, info };

    return JSGraphicsContext::New<SDLGraphicsContext>(env, ci[0]);
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
}

static napi_value GetScanCodeState(napi_env env, napi_callback_info info) noexcept {
  try {
    Napi::CallbackInfo ci{ env, info };
    bool result = ci[0].IsNumber() && sPlugin->GetScanCodeState(ci[0].As<Napi::Number>().Int32Value());

    return Napi::Boolean::New(env, result);
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
}

static napi_value LoadGameControllerMappings(napi_env env, napi_callback_info info) noexcept {
  try {
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
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
}

static napi_value GetGameControllerMapping(napi_env env, napi_callback_info info) noexcept {
  try {
    Napi::CallbackInfo ci{ env, info };
    auto mapping{ sPlugin->GetGameControllerMapping(Napi::CopyUtf8(ci[0])) };

    return Napi::String::New(env, mapping);
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
}

static napi_value ProcessEvents(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return Napi::Boolean::New(env, sPlugin->ProcessEvents()))
}

static napi_value GetGamepadInstanceIds(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return Napi::ToArray<Napi::Number>(env, sPlugin->GetGamepadInstanceIds()))
}

static napi_value IsKeyDown(napi_env env, napi_callback_info info) noexcept {
  try {
    Napi::CallbackInfo ci(env, info);

    bool result = ci[0].IsNumber()
        && ci[1].IsNumber()
        && sPlugin->IsKeyDown(ci[0].As<Number>(), ci[1].As<Number>());

    return Napi::Boolean::New(env, result);
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
}

static napi_value GetAnalogValue(napi_env env, napi_callback_info info) noexcept {
  try {
    Napi::CallbackInfo ci(env, info);

    if (ci[0].IsNumber() && ci[1].IsNumber()) {
      return Napi::NewNumber(env, sPlugin->GetAnalogValue(ci[0].As<Number>(), ci[1].As<Number>()));
    }

    return Napi::NewNumber(env, 0.f);
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
}

static napi_value GetButtonState(napi_env env, napi_callback_info info) noexcept {
  try {
    Napi::CallbackInfo ci(env, info);

    bool result = ci[0].IsNumber()
        && ci[1].IsNumber()
        && sPlugin->GetButtonState(ci[0].As<Number>(), ci[1].As<Number>());

    return Napi::Boolean::New(env, result);
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
}

static napi_value GetAxisState(napi_env env, napi_callback_info info) noexcept {
  try {
    Napi::CallbackInfo ci(env, info);

    if (ci[0].IsNumber() && ci[1].IsNumber()) {
      return Napi::NewNumber(env, sPlugin->GetAxisState(ci[0].As<Number>(), ci[1].As<Number>()));
    }

    return Napi::NewNumber(env, 0.f);
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
}

static napi_value GetHatState(napi_env env, napi_callback_info info) noexcept {
  try {
    Napi::CallbackInfo ci(env, info);

    if (ci[0].IsNumber() && ci[1].IsNumber()) {
      return Napi::NewNumber(env, sPlugin->GetHatState(ci[0].As<Number>(), ci[1].As<Number>()));
    }

    return Napi::NewNumber(env, 0);
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
}

static napi_value GetGamepadInfo(napi_env env, napi_callback_info info) noexcept {
  try {
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
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
    return {};
  }
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

static void BindOnKeyboardScanCode(int32_t scanCode, bool pressed , bool repeat) noexcept {
  InvokeCallback("onKeyboardScanCode", OnKeyboardScanCode, scanCode, pressed, repeat);
}

static void BindOnGamepadStatus(int32_t instanceId, bool connected) noexcept {
  InvokeCallback("onGamepadStatus", OnGamepadStatus, instanceId, connected);
}

static void BindOnGamepadAxis(int32_t instanceId, int32_t axis, float value) noexcept {
  InvokeCallback("onGamepadAxis", OnGamepadAxis, instanceId, axis, value);
};

static void BindOnGamepadAxisMapped(int32_t instanceId, int32_t axis, float value) noexcept {
  InvokeCallback("onGamepadAxisMapped", OnGamepadAxisMapped, instanceId, axis, value);
};

static void BindOnGamepadHat(int32_t instanceId, int32_t hat, int32_t value) noexcept {
  InvokeCallback("onGamepadHat", OnGamepadHat, instanceId, hat, value);
};

static void BindOnGamepadButton(int32_t instanceId, int32_t button, bool pressed) noexcept {
  InvokeCallback("onGamepadButton", OnGamepadButton, instanceId, button, pressed);
};

static void BindOnGamepadButtonMapped(int32_t instanceId, int32_t button, bool pressed) noexcept {
  InvokeCallback("onGamepadButtonMapped", OnGamepadButtonMapped, instanceId, button, pressed);
};

static bool BindOnQuit() noexcept {
  auto& callback{ sPluginCallbacks[OnQuit] };

  if (!callback.IsEmpty()) {
    try {
      Napi::SafeHandleScope scope(callback.Env());
      auto result{ callback({}) };

      return result.ToBoolean();
    } catch (const std::exception& e) {
      auto LAMBDA_FUNCTION = "onQuit";
      LOG_ERROR_LAMBDA("Unhanded JS exception: %s", e.what());
    }
  }

  return true;
};

static void Init(const Napi::Env& env) {
  if (sPlugin) {
    throw Napi::Error::New(env, "SDL platform plugin has already been loaded.");
  }

  NAPI_TRY(env, SDL2::Open());

  sPlugin = new SDLPlatformPlugin();

  sPlugin->onKeyboardScanCode = &BindOnKeyboardScanCode;
  sPlugin->onGamepadStatus = &BindOnGamepadStatus;
  sPlugin->onGamepadAxis = &BindOnGamepadAxis;
  sPlugin->onGamepadAxisMapped = &BindOnGamepadAxisMapped;
  sPlugin->onGamepadHat = &BindOnGamepadHat;
  sPlugin->onGamepadButton = &BindOnGamepadButton;
  sPlugin->onGamepadButtonMapped = &BindOnGamepadButtonMapped;
  sPlugin->onQuit = &BindOnQuit;

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
      // Plugin API
      .WithValue("type", "platform")
      .WithMethod("attach", &Attach)
      .WithMethod("detach", &Detach)
      .WithMethod("destroy", &Destroy)
      .WithMethod("isAttached", &IsAttached)
      // System / Capabilities API
      .WithMethod("getDisplays", &GetDisplays)
      .WithMethod("getVideoDriverNames", &GetVideoDriverNames)
      .WithMethod("loadGameControllerMappings", &LoadGameControllerMappings)
      .WithMethod("getGameControllerMapping", &GetGameControllerMapping)
      .WithMethod("getGamepadInstanceIds", &GetGamepadInstanceIds)
      // Window API
      .WithMethod("createGraphicsContext", &CreateGraphicsContext)
      .WithMethod("processEvents", &ProcessEvents)
      // Keyboard API
      .WithMethod("getScanCodeState", &GetScanCodeState)
      // Gamepad API
      .WithMethod("isKeyDown", &IsKeyDown)
      .WithMethod("getAnalogValue", &GetAnalogValue)
      .WithMethod("getButtonState", &GetButtonState)
      .WithMethod("getAxisState", &GetAxisState)
      .WithMethod("getHatState", &GetHatState)
      .WithMethod("getGamepadInfo", &GetGamepadInfo)
      // Plugin Event Handlers
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
