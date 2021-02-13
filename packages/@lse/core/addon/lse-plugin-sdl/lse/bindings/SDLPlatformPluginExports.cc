/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLPlatformPluginExports.h"

#include <algorithm>
#include <napix.h>
#include <napi-ext.h>
#include <lse/Habitat.h>
#include <lse/Log.h>
#include <lse/SDLGraphicsContext.h>
#include <lse/SDLPlatformPlugin.h>
#include <lse/bindings/CSDLGraphicsContext.h>

using Napi::Boolean;
using Napi::Number;

using napix::descriptor::instance_value;
using napix::descriptor::instance_method;

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
  NAPIX_TRY_STD(env, sPlugin->Attach(), {});
  return {};
}

static napi_value Detach(napi_env env, napi_callback_info info) noexcept {
  ResetCallbacks();
  NAPIX_TRY_STD(env, sPlugin->Detach(), {});
  return {};
}

static napi_value IsAttached(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value(env, sPlugin->IsAttached());
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  ResetCallbacks();
  NAPIX_TRY_STD(env, sPlugin->Destroy(), {});
  return {};
}

static napi_value GetDisplays(napi_env env, napi_callback_info info) noexcept {
  auto NewDisplayMode = [](napi_env env, const SDLDisplayMode& displayMode) {
    return napix::object_new(env, {
        napix::descriptor::instance_value(env, "width", displayMode.width),
        napix::descriptor::instance_value(env, "height", displayMode.height),
    });
  };

  auto displays{sPlugin->GetDisplays()};
  auto result{napix::array_new(env, displays.size())};
  uint32_t resultIndex{0};

  for (const auto& display : sPlugin->GetDisplays()) {
    auto modes = napix::array_from<const SDLDisplayMode&>(env, display.modes, NewDisplayMode);

    auto displayMode = napix::object_new(env, {
        napix::descriptor::instance_value(env, "id", display.id),
        napix::descriptor::instance_value(env, "name", display.name.c_str()),
        napix::descriptor::instance_value("defaultMode", NewDisplayMode(env, display.defaultMode)),
        napix::descriptor::instance_value("modes", modes),
    });

    napi_set_element(env, result, resultIndex++, displayMode);
  }

  return result;
}

static napi_value GetVideoDriverNames(napi_env env, napi_callback_info info) noexcept {
  auto names{sPlugin->GetVideoDriverNames()};

  return napix::array_from<const std::string&>(env, names, napix::to_value);
}

static void ResetCallbacks() noexcept {
  for (auto& sPluginEventCallback : sPluginCallbacks) {
    sPluginEventCallback = Napi::FunctionReference();
  }
}

static napi_value ResetCallbacks(napi_env env, napi_callback_info info) noexcept {
  ResetCallbacks();
  return {};
}

static napi_value CreateGraphicsContext(napi_env env, napi_callback_info info) noexcept {
  auto constructor{Habitat::GetClass(env, Habitat::Class::CGraphicsContext)};

  NAPIX_EXPECT_NOT_NULL(env, constructor, "No GraphicsContext class installed", {});

  auto ci{napix::get_callback_info<1>(env, info)};
  napi_value graphicsContext{};

  auto status = napi_new_instance(env, constructor, ci.arg_count, &ci.args[0], &graphicsContext);

  if (status == napi_ok) {
    return graphicsContext;
  }

  if (!napix::has_pending_exception(env)) {
    napix::throw_error(env, "Could not create GraphicsContext instance");
  }

  return {};
}

static napi_value GetScanCodeState(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<2>(env, info) };
  auto state{ sPlugin->GetScanCodeState(napix::as_int32(env, ci[0], -1)) };

  return napix::to_value_or_null(env, state);
}

static napi_value LoadGameControllerMappings(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  int32_t count;

  if (napix::is_string(env, ci[0])) {
    auto mappings{napix::as_string_utf8(env, ci[0])};

    count = sPlugin->LoadGameControllerMappings(mappings.c_str(), mappings.size());
  } else if (napix::is_buffer(env, ci[0])) {
    auto buffer{napix::as_buffer(env, ci[0])};

    if (buffer.empty()) {
      count = 0;
    } else {
      count = sPlugin->LoadGameControllerMappings(buffer.data, buffer.size);
    }
  } else {
    count = 0;
  }

  return napix::to_value(env, count);
}

static napi_value GetGameControllerMapping(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<1>(env, info)};
  auto mapping{ sPlugin->GetGameControllerMapping(napix::as_string_utf8(env, ci[0]).c_str()) };

  return napix::to_value_or_null(env, mapping);
}

static napi_value ProcessEvents(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, sPlugin->ProcessEvents());
}

static napi_value GetGamepadInstanceIds(napi_env env, napi_callback_info info) noexcept {
  return napix::array_from<int32_t>(env, sPlugin->GetGamepadInstanceIds(), napix::to_value);
}

static napi_value IsKeyDown(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<2>(env, info) };
  auto state{ sPlugin->IsKeyDown(napix::as_int32(env, ci[0], -1), napix::as_int32(env, ci[1], -1)) };

  return napix::to_value_or_null(env, state);
}

static napi_value GetAnalogValue(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<2>(env, info) };
  auto state{ sPlugin->GetAnalogValue(napix::as_int32(env, ci[0], -1), napix::as_int32(env, ci[1], -1)) };

  return napix::to_value_or_null(env, state);
}

static napi_value GetButtonState(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<2>(env, info) };
  auto state{ sPlugin->GetButtonState(napix::as_int32(env, ci[0], -1), napix::as_int32(env, ci[1], -1)) };

  return napix::to_value_or_null(env, state);
}

static napi_value GetAxisState(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<2>(env, info) };
  auto state{ sPlugin->GetAxisState(napix::as_int32(env, ci[0], -1), napix::as_int32(env, ci[1], -1)) };

  return napix::to_value_or_null(env, state);
}

static napi_value GetHatState(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<2>(env, info) };
  auto state{ sPlugin->GetHatState(napix::as_int32(env, ci[0], -1), napix::as_int32(env, ci[1], -1)) };

  return napix::to_value_or_null(env, state);
}

static napi_value GetGamepadInfo(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };
  auto instanceId{ napix::as_int32(env, ci[0], -1) };
  auto gamepadInfo{ sPlugin->GetGamepadInfo(instanceId) };

  return napix::object_new(env, {
    napix::descriptor::instance_value(env, "id", gamepadInfo.instanceId),
    napix::descriptor::instance_value(env, "type", "gamepad"),
    napix::descriptor::instance_value(env, "name", gamepadInfo.name),
    napix::descriptor::instance_value(env, "buttonCount", gamepadInfo.buttonCount),
    napix::descriptor::instance_value(env, "hatCount", gamepadInfo.hatCount),
    napix::descriptor::instance_value(env, "axisCount", gamepadInfo.axisCount),
    napix::descriptor::instance_value(env, "uuid", gamepadInfo.uuid),
  });
}

template<typename T, typename ... Args>
static void InvokeCallback(const char* name, T callbackType, Args ... args) noexcept {
  auto& callback{ sPluginCallbacks[callbackType] };

  if (!callback.IsEmpty()) {
    try {
      Napi::SafeHandleScope scope(callback.Env());
      callback({ napix::to_value_or_null(callback.Env(), args)... });
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

static napi_value CreatePluginInstance(napi_env env) {
#define InstanceAccessor(PROP, EVENT)                                       \
  napix::descriptor::instance_accessor(                                     \
    PROP,                                                                   \
    [](napi_env, napi_callback_info) -> napi_value {                        \
      return sPluginCallbacks[EVENT].Value();                               \
    },                                                                      \
    [](napi_env env, napi_callback_info info) -> napi_value {               \
      Napi::CallbackInfo ci{ env, info };                                   \
      Napi::AssignFunctionReference(sPluginCallbacks[EVENT], ci[0]);        \
      return nullptr;                                                       \
    })

  auto pluginInstance = napix::object_new(env, {
      // Plugin API
      instance_value(env, "type", "platform"),
      instance_method("attach", &Attach),
      instance_method("detach", &Detach),
      instance_method("destroy", &Destroy),
      instance_method("isAttached", &IsAttached),
      // System / Capabilities API
      instance_method("getDisplays", &GetDisplays),
      instance_method("getVideoDriverNames", &GetVideoDriverNames),
      instance_method("loadGameControllerMappings", &LoadGameControllerMappings),
      instance_method("getGameControllerMapping", &GetGameControllerMapping),
      instance_method("getGamepadInstanceIds", &GetGamepadInstanceIds),
      // Window API
      instance_method("createGraphicsContext", &CreateGraphicsContext),
      instance_method("processEvents", &ProcessEvents),
      // Keyboard API
      instance_method("getScanCodeState", &GetScanCodeState),
      // Gamepad API
      instance_method("isKeyDown", &IsKeyDown),
      instance_method("getAnalogValue", &GetAnalogValue),
      instance_method("getButtonState", &GetButtonState),
      instance_method("getAxisState", &GetAxisState),
      instance_method("getHatState", &GetHatState),
      instance_method("getGamepadInfo", &GetGamepadInfo),
      // Plugin Event Handlers
      instance_method("resetCallbacks", &ResetCallbacks),
      InstanceAccessor("onKeyboardScanCode", OnKeyboardScanCode),
      InstanceAccessor("onGamepadStatus", OnGamepadStatus),
      InstanceAccessor("onGamepadAxis", OnGamepadAxis),
      InstanceAccessor("onGamepadAxisMapped", OnGamepadAxisMapped),
      InstanceAccessor("onGamepadHat", OnGamepadHat),
      InstanceAccessor("onGamepadButton", OnGamepadButton),
      InstanceAccessor("onGamepadButtonMapped", OnGamepadButtonMapped),
      InstanceAccessor("onQuit", OnQuit)
  });

  return pluginInstance;

#undef InstanceAccessor
}

napi_value LoadSDLPlatformPlugin(napi_env env) noexcept {
  auto hasGraphicsContext{Habitat::HasClass(env, Habitat::Class::CGraphicsContext)};

  NAPIX_EXPECT_FALSE(env, hasGraphicsContext, "GraphicsContext class already installed.", {});
  NAPIX_EXPECT_NULL(env, sPlugin, "SDL platform plugin has already been loaded.", {});

  try {
    SDL2::Open();
  } catch (const std::exception& e) {
    napix::throw_error(env, e.what());
    return {};
  }

  auto jsPluginInstance = CreatePluginInstance(env);

  if (napix::has_pending_exception(env)) {
    return {};
  }

  NAPIX_EXPECT_NOT_NULL(env, jsPluginInstance, "Failed to create SDL platform plugin instance", {});

  Habitat::SetClass(env, Habitat::Class::CGraphicsContext, CSDLGraphicsContext::CreateClass(env));

  if (napix::has_pending_exception(env)) {
    return {};
  }

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

  return jsPluginInstance;
}

} // namespace bindings
} // namespace lse
