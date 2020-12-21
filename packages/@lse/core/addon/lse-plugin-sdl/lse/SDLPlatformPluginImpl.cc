/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/SDLPlatformPluginImpl.h>

#include <lse/CapabilitiesView.h>
#include <lse/string-ext.h>
#include <lse/Log.h>
#include <lse/Timer.h>
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

// Note, SDL_JOYSTICK_AXIS_* defines were introduced after 2.0.4.

constexpr auto SDL_JOYSTICK_AXIS_MIN_F =
#ifndef SDL_JOYSTICK_AXIS_MIN
    -32768.f;
#else
    static_cast<float>(SDL_JOYSTICK_AXIS_MIN);
#endif

constexpr auto SDL_JOYSTICK_AXIS_MAX_F =
#ifndef SDL_JOYSTICK_AXIS_MAX
    32767.f;
#else
    static_cast<float>(SDL_JOYSTICK_AXIS_MAX);
#endif

std::unordered_map<std::string, SDLPlatformPluginImpl::StageCallback> SDLPlatformPluginImpl::callbackMap{
    { "connected", StageCallbackGamepadConnected },
    { "disconnected", StageCallbackGamepadDisconnected },
    { "keyup", StageCallbackKeyboardKeyUp },
    { "keydown", StageCallbackKeyboardKeyDown },
    { "buttonup", StageCallbackGamepadButtonUp },
    { "buttondown", StageCallbackGamepadButtonDown },
    { "hatup", StageCallbackHatUp },
    { "hatdown", StageCallbackHatDown },
    { "axismotion", StageCallbackGamepadAxisMotion },
    { "quit", StageCallbackQuit },
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

  if (!info[0].IsString()) {
    return Boolean::New(env, false);
  }

  auto result = SDL2::SDL_GameControllerAddMappingsFromRW(
      SDL2::SDL_RWFromFile(info[0].As<String>().Utf8Value().c_str(), "rb"), 1);

  return Boolean::New(env, result >= 0);
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

  SDL2::SDL_GameControllerEventState(SDL_IGNORE);
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
        this->DispatchKeyboardKeyUp(env, event.key.keysym.scancode);
        break;
      case SDL_KEYDOWN:
        this->DispatchKeyboardKeyDown(env, event.key.keysym.scancode, event.key.repeat != 0);
        break;
      case SDL_JOYBUTTONUP:
        this->DispatchJoystickButtonUp(env, event.jbutton.which, event.jbutton.button);
        break;
      case SDL_JOYBUTTONDOWN:
        this->DispatchJoystickButtonDown(env, event.jbutton.which, event.jbutton.button);
        break;
      case SDL_JOYHATMOTION:
        this->DispatchJoystickHatMotion(env, event.jhat.which, event.jhat.hat, event.jhat.value);
        break;
      case SDL_JOYAXISMOTION:
        this->DispatchJoystickAxisMotion(env, event.jaxis.which, event.jaxis.axis,
                                         static_cast<float>(event.jaxis.value));
        break;
      case SDL_JOYDEVICEADDED:
        this->DispatchJoystickAdded(env, event.jdevice.which);
        break;
      case SDL_JOYDEVICEREMOVED:
        this->DispatchJoystickRemoved(env, event.jdevice.which);
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
  if (!this->IsCallbackEmpty(StageCallbackQuit)) {
    HandleScope scope(env);

    this->Call(StageCallbackQuit, {});
  }

  // exit on quit
  // TODO: allow user to override
  return false;
}

void SDLPlatformPluginImpl::DispatchKeyboardKeyDown(Napi::Env env, int32_t scanCode, bool isRepeat) {
  if (!this->IsCallbackEmpty(StageCallbackKeyboardKeyDown)) {
    HandleScope scope(env);

    this->Call(StageCallbackKeyboardKeyDown, {
        this->keyboard->Value(),
        Number::New(env, scanCode),
        Boolean::New(env, isRepeat),
    });
  }
}

void SDLPlatformPluginImpl::DispatchKeyboardKeyUp(Napi::Env env, int32_t scanCode) {
  if (!this->IsCallbackEmpty(StageCallbackKeyboardKeyUp)) {
    HandleScope scope(env);

    this->Call(StageCallbackKeyboardKeyUp, {
        this->keyboard->Value(),
        Number::New(env, scanCode)
    });
  }
}

void SDLPlatformPluginImpl::DispatchJoystickButtonUp(Napi::Env env, int32_t instanceId, int32_t buttonId) {
  if (!this->IsCallbackEmpty(StageCallbackGamepadButtonUp)) {
    HandleScope scope(env);

    this->Call(StageCallbackGamepadButtonUp, {
        this->GetGamepad(env, instanceId),
        Number::New(env, buttonId),
    });
  }
}

void SDLPlatformPluginImpl::DispatchJoystickButtonDown(Napi::Env env, int32_t instanceId, int32_t buttonId) {
  if (!this->IsCallbackEmpty(StageCallbackGamepadButtonDown)) {
    HandleScope scope(env);

    this->Call(StageCallbackGamepadButtonDown, {
        this->GetGamepad(env, instanceId),
        Number::New(env, buttonId),
    });
  }
}

void SDLPlatformPluginImpl::DispatchJoystickAxisMotion(
    Napi::Env env, int32_t instanceId, uint8_t axisIndex, float value) {
  if (!this->IsCallbackEmpty(StageCallbackGamepadAxisMotion)) {
    HandleScope scope(env);

    this->Call(StageCallbackGamepadAxisMotion, {
        this->GetGamepad(env, instanceId),
        Number::New(env, axisIndex),
        Number::New(env, value < 0 ? -(value / SDL_JOYSTICK_AXIS_MIN_F) : value / SDL_JOYSTICK_AXIS_MAX_F),
    });
  }
}

void SDLPlatformPluginImpl::DispatchJoystickHatMotion(
    Napi::Env env, int32_t instanceId, uint8_t hatIndex, uint8_t hatValue) {
  auto p{ this->gamepadsByInstanceId.find(instanceId) };

  if (p == this->gamepadsByInstanceId.end()) {
    return;
  }

  auto gamepad{ p->second };
  auto state{ gamepad->GetHatState(hatIndex) };
  HandleScope scope(env);

  if (state != 0) {
    gamepad->SetHatState(hatIndex, 0);
    if (!IsCallbackEmpty(StageCallbackHatUp)) {
      Call(StageCallbackHatUp, {
          gamepad->Value(),
          Number::New(env, hatIndex),
          Number::New(env, state),
      });
    }
  }

  switch (hatValue) {
    case SDL_HAT_UP:
    case SDL_HAT_RIGHT:
    case SDL_HAT_DOWN:
    case SDL_HAT_LEFT:
      gamepad->SetHatState(hatIndex, hatValue);
      if (!IsCallbackEmpty(StageCallbackHatDown)) {
        Call(StageCallbackHatDown, {
            gamepad->Value(),
            Number::New(env, hatIndex),
            Number::New(env, hatValue),
        });
      }
      break;
    default:
      break;
  }
}

void SDLPlatformPluginImpl::DispatchJoystickAdded(Napi::Env env, int32_t index) {
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

  if (!IsCallbackEmpty(StageCallbackGamepadConnected)) {
    HandleScope scope(env);

    Call(StageCallbackGamepadConnected, { gamepad->Value() });
  }
}

void SDLPlatformPluginImpl::DispatchJoystickRemoved(Napi::Env env, int32_t instanceId) {
  auto p{ this->gamepadsByInstanceId.find(instanceId) };

  if (p == this->gamepadsByInstanceId.end()) {
    return;
  }

  auto gamepad{ p->second };

  this->gamepadsByInstanceId.erase(p);

  if (!IsCallbackEmpty(StageCallbackGamepadDisconnected)) {
    HandleScope scope(env);

    Call(StageCallbackGamepadDisconnected, { gamepad->Value() });
  }

  gamepad->Destroy();
  gamepad->Unref();
}

void SDLPlatformPluginImpl::Call(StageCallback callbackId, const std::initializer_list<napi_value>& args) {
  try {
    this->callbacks[callbackId](args);
  } catch (const std::exception& e) {
    LOG_ERROR("Callback (%i) uncaught JS exception: %s", callbackId, e);
  }
}

bool SDLPlatformPluginImpl::IsCallbackEmpty(StageCallback callbackId) {
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
