/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/SDLPlatformPluginImpl.h>

#include <lse/CapabilitiesView.h>
#include <lse/string-ext.h>
#include <lse/Log.h>
#include <lse/SDLGamepad.h>
#include <lse/SDLGraphicsContextImpl.h>
#include <lse/SDLKeyboard.h>
#include <unordered_set>
#include <lse/SDL2.h>

using Napi::Array;
using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::Persistent;
using Napi::SafeObjectWrap;
using Napi::String;
using Napi::Value;

bool operator==(SDL_DisplayMode const& lhs, SDL_DisplayMode const& rhs) {
  return (lhs.w == rhs.w) && (lhs.h == rhs.h);
}

struct SDL_DisplayModeHash {
  std::size_t operator()(const SDL_DisplayMode& displayMode) const noexcept {
    return (static_cast<std::size_t>(displayMode.w) << 32) | static_cast<std::size_t>(displayMode.h);
  }
};

namespace lse {

std::unordered_map<std::string, SDLPlatformPluginImpl::PluginCallback> SDLPlatformPluginImpl::callbackMap{
    { "keyboard:button", PluginCallbackKeyboardButton },
    { "gamepad:status", PluginCallbackGamepadStatus },
    { "gamepad:axis", PluginCallbackGamepadAxis },
    { "gamepad:hat", PluginCallbackGamepadHat },
    { "gamepad:button", PluginCallbackGamepadButton },
    { "gamepad:button-mapped", PluginCallbackGamepadButtonMapped },
    { "gamepad:axis-mapped", PluginCallbackGamepadAxisMapped },
    { "quit", PluginCallbackQuit },
};

SDLPlatformPluginImpl::SDLPlatformPluginImpl(const CallbackInfo& info) {
  auto env{ info.Env() };
  HandleScope scope(env);

  SDL_version compiled{};
  SDL_version linked{};

  SDL_VERSION(&compiled);
  SDL2::SDL_GetVersion(&linked);

  LOGX_INFO("SDL Version %i.%i.%i (compiled=%i.%i.%i)",
            linked.major, linked.minor, linked.patch, compiled.major, compiled.minor, compiled.patch);

  this->Init(env);

  this->capabilities = this->DetermineCapabilities(env);
  this->capabilitiesRef = Persistent(ToCapabilitiesView(env, this->capabilities));

  std::string videoDrivers;

  for (const auto& videoDriver : this->capabilities.videoDrivers) {
    if (!videoDrivers.empty()) {
      videoDrivers += ", ";
    }
    videoDrivers += videoDriver;
  }

  LOGX_INFO("Default Video Driver: %s", SDL2::SDL_GetCurrentVideoDriver());
  LOGX_INFO("Video Drivers: %s", videoDrivers);
}

SDLPlatformPluginImpl::~SDLPlatformPluginImpl() {
  // TODO: cleanup?
}

Value SDLPlatformPluginImpl::GetKeyboard(const CallbackInfo& info) {
  return this->keyboard->Value();
}

Value SDLPlatformPluginImpl::GetGamepads(const CallbackInfo& info) {
  auto env{ info.Env() };
  EscapableHandleScope scope(env);
  auto array{ Array::New(env, this->gamepadsByInstanceId.size()) };
  auto index{ 0u };

  for (auto& p : this->gamepadsByInstanceId) {
    array.Set(index++, p.second->Value());
  }

  return scope.Escape(array);
}

Value SDLPlatformPluginImpl::GetCapabilities(const Napi::CallbackInfo& info) {
  return this->capabilitiesRef.Value();
}

Capabilities SDLPlatformPluginImpl::DetermineCapabilities(Napi::Env env) {
  Capabilities caps{};
  auto numVideoDisplays{ SDL2::SDL_GetNumVideoDisplays() };

  if (numVideoDisplays < 0) {
    LOG_ERROR("SDL_GetNumVideoDisplays: %s", SDL2::SDL_GetError());
    return caps;
  }

  std::unordered_set<SDL_DisplayMode, SDL_DisplayModeHash> uniqueDisplayModes;

  for (auto i{ 0 }; i < numVideoDisplays; i++) {
    Display display{};
    auto name{ SDL2::SDL_GetDisplayName(i) };

    display.name = name ? name : "";

    auto numDisplayModes{ SDL2::SDL_GetNumDisplayModes(i) };

    if (numDisplayModes < 0) {
      LOG_ERROR("SDL_GetNumDisplayModes(%i: %s", i, SDL2::SDL_GetError());
      continue;
    }

    SDL_DisplayMode desktopDisplayMode;

    if (SDL2::SDL_GetDesktopDisplayMode(i, &desktopDisplayMode) != 0) {
      LOG_ERROR("SDL_GetDesktopDisplayMode(%i): %s", i, SDL2::SDL_GetError());
      continue;
    }

    SDL_DisplayMode displayMode;

    display.defaultMode = { desktopDisplayMode.w, desktopDisplayMode.h };
    uniqueDisplayModes.clear();

    for (auto j{ 0 }; j < numDisplayModes; j++) {
      if (SDL2::SDL_GetDisplayMode(i, j, &displayMode) != 0) {
        LOG_ERROR("SDL_GetDisplayMode(%i, %i): %s", i, j, SDL2::SDL_GetError());
        uniqueDisplayModes.clear();
        break;
      }

      uniqueDisplayModes.insert(displayMode);
    }

    for (auto& p : uniqueDisplayModes) {
      display.modes.push_back({ p.w, p.h });
    }

    caps.displays.emplace_back(display);
  }

  auto numVideoDrivers = SDL2::SDL_GetNumVideoDrivers();

  for (auto i = 0; i < numVideoDrivers; i++) {
    caps.videoDrivers.push_back(SDL2::SDL_GetVideoDriver(i));
  }

  return caps;
}

Napi::Value SDLPlatformPluginImpl::LoadGameControllerMappings(const Napi::CallbackInfo& info) {
  auto env{ info.Env() };
  auto value{ info[0] };
  std::string tempString;
  SDL_RWops* ops;

  if (value.IsString()) {
    auto valueLen{ Napi::StringByteLength(value) };

    if (valueLen < Napi::SizeOfCopyUtf8Buffer()) {
      ops = SDL2::SDL_RWFromMem(Napi::CopyUtf8(value), valueLen);
    } else {
      tempString = value.As<String>();
      ops = SDL2::SDL_RWFromConstMem(tempString.c_str(), tempString.size());
    }
  } else if (value.IsBuffer()) {
    auto buffer{ value.As<Napi::Buffer<uint8_t>>() };

    ops = SDL2::SDL_RWFromMem(buffer.Data(), buffer.Length());
  } else {
    return Number::New(env, 0);
  }

  auto result = SDL2::SDL_GameControllerAddMappingsFromRW(ops, 1);

  if (result >= 0) {
    this->SyncGamepads(env);
  }

  return Number::New(env, result);
}

Napi::Value SDLPlatformPluginImpl::GetGameControllerMapping(const Napi::CallbackInfo& info) {
  auto env{ info.Env() };
  auto guid{ SDL2::SDL_JoystickGetGUIDFromString(Napi::CopyUtf8(info[0])) };
  auto mapping{ SDL2::SDL_GameControllerMappingForGUID(guid) };

  if (mapping) {
    return Napi::String::New(env, mapping);
  }

  return env.Undefined();
}

void SDLPlatformPluginImpl::SetCallback(const CallbackInfo& info) {
  auto env{ info.Env() };
  auto id{ info[0].As<String>().Utf8Value() };
  auto it{ callbackMap.find(info[0].As<String>().Utf8Value()) };

  if (it == callbackMap.end()) {
    throw Error::New(env, Format("Unknown callback id: %s", id));
  }

  if (info[1].IsFunction()) {
    this->callbacks[it->second].Reset(info[1].As<Function>(), 1);
  } else {
    this->callbacks[it->second].Reset();
  }
}

void SDLPlatformPluginImpl::ResetCallbacks(const CallbackInfo& info) {
  for (auto& callback : this->callbacks) {
    callback.Reset();
  }
}

void SDLPlatformPluginImpl::Init(Napi::Env env) {
  if (SDL2::SDL_WasInit(SDL_INIT_VIDEO) == 0 && SDL2::SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw Error::New(env, Format("Failed to init SDL video. SDL Error: %s", SDL2::SDL_GetError()));
  }

  SDL2::SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  if (SDL2::SDL_WasInit(SDL_INIT_JOYSTICK) == 0 && SDL2::SDL_Init(SDL_INIT_JOYSTICK) != 0) {
    throw Error::New(env, Format("Failed to init SDL joystick. SDL Error: %s", SDL2::SDL_GetError()));
  }

  if (SDL2::SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0 && SDL2::SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
    throw Error::New(env, Format("Failed to init SDL gamecontroller. SDL Error: %s", SDL2::SDL_GetError()));
  }
}

void SDLPlatformPluginImpl::Attach(const CallbackInfo& info) {
  if (this->isAttached) {
    return;
  }

  auto env{ info.Env() };
  HandleScope scope(env);

  this->Init(env);
  this->SyncGamepads(env);

  if (!this->keyboard) {
    this->keyboard = SDLKeyboard::CastRef(SDLKeyboard::GetClass(env).New({}));
  }

  this->isAttached = true;
}

void SDLPlatformPluginImpl::Detach(const CallbackInfo& info) {
  if (!this->isAttached) {
    return;
  }

  this->ClearGamepads();

  SDL2::SDL_Quit();

  this->isAttached = false;
}

void SDLPlatformPluginImpl::Destroy(const CallbackInfo& info) {
  this->ResetCallbacks(info);
  this->capabilitiesRef.Reset();

  if (this->keyboard) {
    this->keyboard->Unref();
    this->keyboard = nullptr;
  }

  this->Detach(info);
}

Value SDLPlatformPluginImpl::CreateGraphicsContext(const Napi::CallbackInfo& info) {
  return GraphicsContext::Create<SDLGraphicsContextImpl>(info.Env(), info[0]);
}

Value SDLPlatformPluginImpl::ProcessEvents(const Napi::CallbackInfo& info) {
  static constexpr auto NUM_EVENTS_PER_FRAME{ 20 };
  static SDL_Event eventBuffer[NUM_EVENTS_PER_FRAME];

  SDL2::SDL_PumpEvents();

  auto env{ info.Env() };
  auto eventIndex{ 0 };
  auto eventCount{ SDL2::SDL_PeepEvents(
      eventBuffer, NUM_EVENTS_PER_FRAME, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) };
  auto result{ true };

  if (eventCount < 0) {
    LOG_ERROR(SDL2::SDL_GetError());
    result = false;
  }

  while (result && eventIndex < eventCount) {
    if (!this->isAttached) {
      break;
    }

    const auto& event{ eventBuffer[eventIndex++] };

    switch (event.type) {
      case SDL_QUIT:
        result = this->DispatchQuit(env);
        break;
      case SDL_KEYUP:
      case SDL_KEYDOWN:
        this->DispatchKeyboardButton(env, event.key.keysym.scancode, event.key.state, event.key.repeat != 0);
        break;
      case SDL_JOYBUTTONUP:
      case SDL_JOYBUTTONDOWN:
        this->DispatchGamepadButton(env, event.jbutton.which, event.jbutton.button, event.jbutton.state);
        break;
      case SDL_JOYHATMOTION:
        this->DispatchGamepadHat(env, event.jhat.which, event.jhat.hat, event.jhat.value);
        break;
      case SDL_JOYAXISMOTION:
        this->DispatchGamepadAxis(env, event.jaxis.which, event.jaxis.axis, static_cast<float>(event.jaxis.value));
        break;
      case SDL_JOYDEVICEADDED:
        this->DispatchGamepadConnected(env, event.jdevice.which);
        break;
      case SDL_JOYDEVICEREMOVED:
        this->DispatchGamepadDisconnected(env, event.jdevice.which);
        break;
      case SDL_CONTROLLERAXISMOTION:
        this->DispatchGamepadAxisMapped(env,
                                        event.jaxis.which,
                                        event.jaxis.axis,
                                        static_cast<float>(event.jaxis.value));
        break;
      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
        this->DispatchGamepadButtonMapped(env, event.cbutton.which, event.cbutton.button, event.cbutton.state);
        break;
      default:
        break;
    }
  }

  return Boolean::New(env, result);
}

void SDLPlatformPluginImpl::Finalize() {
  delete this;
}

bool SDLPlatformPluginImpl::DispatchQuit(Napi::Env env) {
  if (!this->IsCallbackEmpty(PluginCallbackQuit)) {
    HandleScope scope(env);

    this->Call(PluginCallbackQuit, {});
  }

  // exit on quit
  // TODO: allow user to override
  return false;
}

void SDLPlatformPluginImpl::DispatchKeyboardButton(
    Napi::Env env,
    int32_t scanCode,
    uint8_t buttonState,
    bool isRepeat) {
  if (!this->IsCallbackEmpty(PluginCallbackKeyboardButton)) {
    HandleScope scope(env);

    this->Call(PluginCallbackKeyboardButton, {
        this->keyboard->Value(),
        Number::New(env, scanCode),
        Number::New(env, buttonState),
        Boolean::New(env, isRepeat),
    });
  }
}

void SDLPlatformPluginImpl::DispatchGamepadButton(
    Napi::Env env,
    int32_t instanceId,
    uint8_t buttonId,
    uint8_t buttonState) {
  if (!this->IsCallbackEmpty(PluginCallbackGamepadButton)) {
    HandleScope scope(env);

    this->Call(PluginCallbackGamepadButton, {
        this->GetGamepad(env, instanceId),
        Number::New(env, buttonId),
        Number::New(env, buttonState)
    });
  }
}

void SDLPlatformPluginImpl::DispatchGamepadAxis(
    Napi::Env env, int32_t instanceId, uint8_t axisIndex, float value) {
  if (!this->IsCallbackEmpty(PluginCallbackGamepadAxis)) {
    HandleScope scope(env);

    this->Call(PluginCallbackGamepadAxis, {
        this->GetGamepad(env, instanceId),
        Number::New(env, axisIndex),
        Number::New(env, value < 0 ? -(value / SDL_JOYSTICK_AXIS_MIN_F) : value / SDL_JOYSTICK_AXIS_MAX_F),
    });
  }
}

void SDLPlatformPluginImpl::DispatchGamepadAxisMapped(
    Napi::Env env, int32_t instanceId, uint8_t axisIndex, float value) {
  if (!this->IsCallbackEmpty(PluginCallbackGamepadAxisMapped)) {
    HandleScope scope(env);

    this->Call(PluginCallbackGamepadAxisMapped, {
        this->GetGamepad(env, instanceId),
        Number::New(env, axisIndex),
        Number::New(env, value < 0 ? -(value / SDL_JOYSTICK_AXIS_MIN_F) : value / SDL_JOYSTICK_AXIS_MAX_F),
    });
  }
}

void SDLPlatformPluginImpl::DispatchGamepadHat(
    Napi::Env env, int32_t instanceId, uint8_t hatIndex, uint8_t hatValue) {
  if (!IsCallbackEmpty(PluginCallbackGamepadHat)) {
    HandleScope scope(env);

    Call(PluginCallbackGamepadHat, {
        this->GetGamepad(env, instanceId),
        Number::New(env, hatIndex),
        Number::New(env, hatValue)
    });
  }
}

void SDLPlatformPluginImpl::DispatchGamepadConnected(Napi::Env env, int32_t index) {
  auto joystick{ SDL2::SDL_JoystickOpen(index) };

  if (!joystick) {
    // TODO: ????
    return;
  }

  auto instanceId{ SDL2::SDL_JoystickInstanceID(joystick) };

  SDL2::SDL_JoystickClose(joystick);

  if (this->gamepadsByInstanceId.find(instanceId) != this->gamepadsByInstanceId.end()) {
    return;
  }

  // TODO: exception
  auto gamepad{ this->AddGamepad(env, index) };

  if (!IsCallbackEmpty(PluginCallbackGamepadStatus)) {
    HandleScope scope(env);

    Call(PluginCallbackGamepadStatus, { gamepad->Value(), Boolean::New(env, true) });
  }
}

void SDLPlatformPluginImpl::DispatchGamepadDisconnected(Napi::Env env, int32_t instanceId) {
  auto p{ this->gamepadsByInstanceId.find(instanceId) };

  if (p == this->gamepadsByInstanceId.end()) {
    return;
  }

  auto gamepad{ p->second };

  this->gamepadsByInstanceId.erase(p);

  if (!IsCallbackEmpty(PluginCallbackGamepadStatus)) {
    HandleScope scope(env);

    Call(PluginCallbackGamepadStatus, { gamepad->Value(), Boolean::New(env, false) });
  }

  gamepad->Destroy();
  gamepad->Unref();
}

void SDLPlatformPluginImpl::DispatchGamepadButtonMapped(
    Napi::Env env,
    int32_t instanceId,
    uint8_t buttonId,
    uint8_t buttonState) {
  if (!this->IsCallbackEmpty(PluginCallbackGamepadButtonMapped)) {
    HandleScope scope(env);

    this->Call(PluginCallbackGamepadButtonMapped, {
        this->GetGamepad(env, instanceId),
        Number::New(env, buttonId),
        Number::New(env, buttonState)
    });
  }
}

void SDLPlatformPluginImpl::Call(PluginCallback callbackId, const std::initializer_list<napi_value>& args) {
  try {
    this->callbacks[callbackId](args);
  } catch (const std::exception& e) {
    LOG_ERROR("Callback (%i) uncaught JS exception: %s", callbackId, e);
  }
}

bool SDLPlatformPluginImpl::IsCallbackEmpty(PluginCallback callbackId) {
  return this->callbacks[callbackId].IsEmpty();
}

void SDLPlatformPluginImpl::SyncGamepads(Napi::Env env) {
  this->ClearGamepads();

  for (int32_t i{ 0 }; i < SDL2::SDL_NumJoysticks(); i++) {
    try {
      this->AddGamepad(env, i);
    } catch (const std::exception& e) {
      LOG_ERROR("Joystick #%i: %s", i, e);
    }
  }
}

void SDLPlatformPluginImpl::ClearGamepads() {
  for (auto& p : this->gamepadsByInstanceId) {
    p.second->Destroy();
    p.second->Unref();
  }

  this->gamepadsByInstanceId.clear();
}

SDLGamepad* SDLPlatformPluginImpl::AddGamepad(Napi::Env env, int32_t index) {
  HandleScope scope(env);
  auto gamepad{ SDLGamepad::New(env, index) };

  gamepad->Ref();

  try {
    this->gamepadsByInstanceId[gamepad->GetId()] = gamepad;
  } catch (const std::exception&) {
    gamepad->Unref();
  }

  return gamepad;
}

Value SDLPlatformPluginImpl::GetGamepad(Napi::Env env, int32_t instanceId) {
  auto p{ this->gamepadsByInstanceId.find(instanceId) };

  return p == this->gamepadsByInstanceId.end() ? env.Undefined() : p->second->Value();
}

} // namespace lse

#undef CallbackInstanceAccessor
