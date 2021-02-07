/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLPlatformPlugin.h"

#include <lse/string-ext.h>
#include <lse/Log.h>

namespace lse {

static float AxisValueToFloat(float value) noexcept;

SDLPlatformPlugin::SDLPlatformPlugin() {
  this->Attach();
}

void SDLPlatformPlugin::Attach() {
  if (this->isAttached) {
    return;
  }

  if (SDL2::SDL_WasInit(SDL_INIT_VIDEO) == 0 && SDL2::SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw std::runtime_error(Format("Failed to init SDL Video. SDL Error: %s", SDL2::SDL_GetError()));
  }

  SDL2::SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  if (SDL2::SDL_WasInit(SDL_INIT_JOYSTICK) == 0 && SDL2::SDL_Init(SDL_INIT_JOYSTICK) != 0) {
    throw std::runtime_error(Format("Failed to init SDL Joystick. SDL Error: %s", SDL2::SDL_GetError()));
  }

  if (SDL2::SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0 && SDL2::SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
    throw std::runtime_error(Format("Failed to init SDL GameController. SDL Error: %s", SDL2::SDL_GetError()));
  }

  auto count = SDL2::SDL_NumJoysticks();

  for (auto index = 0; index < count; index++) {
    this->AddGamepadByIndex(index);
  }

  this->isAttached = true;
}

void SDLPlatformPlugin::Detach() noexcept {
  if (!this->isAttached) {
    return;
  }

  for (auto instanceId : this->GetGamepadInstanceIds()) {
    this->RemoveGamepad(instanceId);
  }

  this->gamepads.clear();

  SDL2::SDL_Quit();

  this->isAttached = false;
}

void SDLPlatformPlugin::Destroy() noexcept {
  this->Detach();
}

std::vector<std::string> SDLPlatformPlugin::GetVideoDriverNames() {
  auto numVideoDrivers = SDL2::SDL_GetNumVideoDrivers();
  std::vector<std::string> names;

  names.reserve(numVideoDrivers);

  for (auto i = 0; i < numVideoDrivers; i++) {
    names.emplace_back(SDL2::SDL_GetVideoDriver(i));
  }

  return names;
}

std::vector<SDLDisplay> SDLPlatformPlugin::GetDisplays() {
  auto numVideoDisplays{ SDL2::SDL_GetNumVideoDisplays() };
  std::vector<SDLDisplay> displays;

  if (numVideoDisplays <= 0) {
    LOG_ERROR("SDL_GetNumVideoDisplays(%i): %s", SDL2::SDL_GetError());

    return displays;
  } else {
    displays.reserve(numVideoDisplays);
  }

  for (auto i{ 0 }; i < numVideoDisplays; i++) {
    SDL_DisplayMode desktopDisplayMode{};

    if (SDL2::SDL_GetDesktopDisplayMode(i, &desktopDisplayMode) != 0) {
      LOG_ERROR("SDL_GetDesktopDisplayMode(%i): %s", i, SDL2::SDL_GetError());
    }

    auto name{ SDL2::SDL_GetDisplayName(i) };
    SDLDisplay display {
      i,
      name ? name : "",
      {desktopDisplayMode.w, desktopDisplayMode.h},
      {}
    };

    auto numDisplayModes{ SDL2::SDL_GetNumDisplayModes(i) };

    if (numDisplayModes <= 0) {
      LOG_ERROR("SDL_GetNumDisplayModes(%i): %s", i, SDL2::SDL_GetError());
    } else {
      display.modes.reserve(numDisplayModes);
    }

    for (auto j{ 0 }; j < numDisplayModes; j++) {
      SDL_DisplayMode displayMode{};

      if (SDL2::SDL_GetDisplayMode(i, j, &displayMode) != 0) {
        LOG_ERROR("SDL_GetDisplayMode(%i, %i): %s", i, j, SDL2::SDL_GetError());
        display.modes.clear();

        break;
      }

      if (display.modes.back().width != displayMode.w || display.modes.back().height != displayMode.h) {
        display.modes.emplace_back(SDLDisplayMode{ displayMode.w, displayMode.h });
      }
    }

    displays.emplace_back(display);
  }

  return displays;
}

bool SDLPlatformPlugin::GetScanCodeState(int32_t scanCode) noexcept {
  auto len{ 0 };
  auto state{ SDL2::SDL_GetKeyboardState(&len) };

  return (scanCode > 0 && scanCode < len && state && state[scanCode] != 0);
}

const char* SDLPlatformPlugin::GetGameControllerMapping(const char* uuid) noexcept {
  auto guid{ SDL2::SDL_JoystickGetGUIDFromString(uuid) };
  auto mapping{ SDL2::SDL_GameControllerMappingForGUID(guid) };

  return mapping ? mapping : "";
}

bool SDLPlatformPlugin::ProcessEvents() noexcept {
  static constexpr auto NUM_EVENTS_PER_FRAME{ 20 };
  static SDL_Event eventBuffer[NUM_EVENTS_PER_FRAME];

  SDL2::SDL_PumpEvents();

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
        if (this->onQuit) {
          result = !this->onQuit();
        } else {
          result = false;
        }
        break;
      case SDL_KEYUP:
      case SDL_KEYDOWN:
        this->onKeyboardScanCode(event.key.keysym.scancode, event.key.state == SDL_PRESSED, event.key.repeat != 0);
        break;
      case SDL_JOYBUTTONUP:
      case SDL_JOYBUTTONDOWN:
        this->onGamepadButton(event.jbutton.which, event.jbutton.button, event.jbutton.state == SDL_PRESSED);
        break;
      case SDL_JOYHATMOTION:
        this->onGamepadHat(event.jhat.which, event.jhat.hat, event.jhat.value);
        break;
      case SDL_JOYAXISMOTION:
        this->onGamepadAxis(event.jaxis.which, event.jaxis.axis, AxisValueToFloat(event.jaxis.value));
        break;
      case SDL_JOYDEVICEADDED:
        this->DispatchGamepadConnected(event.jdevice.which);
        break;
      case SDL_JOYDEVICEREMOVED:
        this->DispatchGamepadDisconnected(event.jdevice.which);
        break;
      case SDL_CONTROLLERAXISMOTION:
        this->onGamepadAxisMapped(event.jaxis.which, event.jaxis.axis, AxisValueToFloat(event.jaxis.value));
        break;
      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
        this->onGamepadButtonMapped(event.cbutton.which, event.cbutton.button, event.cbutton.state == SDL_PRESSED);
        break;
      default:
        break;
    }
  }

  return result;
}

int32_t SDLPlatformPlugin::LoadGameControllerMappings(const void* utf8, size_t len) noexcept {
  auto ops{ SDL2::SDL_RWFromConstMem(utf8, len) };

  return ops ? SDL2::SDL_GameControllerAddMappingsFromRW(ops, 1) : 0;
}

std::vector<int32_t> SDLPlatformPlugin::GetGamepadInstanceIds() {
  std::vector<int32_t> instanceIds;

  instanceIds.reserve(this->gamepads.size());

  for (auto& entry : this->gamepads) {
    instanceIds.push_back(entry.first);
  }

  return instanceIds;
}

bool SDLPlatformPlugin::IsKeyDown(int32_t instanceId, int32_t key) noexcept {
  return key >= 0
    && key < SDL_CONTROLLER_BUTTON_MAX
    && this->gamepads.contains(instanceId)
    && SDL2::SDL_GameControllerGetButton(
      this->gamepads[instanceId].gameController,
      static_cast<SDL_GameControllerButton>(key)) > 0;
}

float SDLPlatformPlugin::GetAnalogValue(int32_t instanceId, int32_t analogKey) noexcept {
  return analogKey >= 0
      && analogKey < SDL_CONTROLLER_AXIS_MAX
      && this->gamepads.contains(instanceId)
      && SDL2::SDL_GameControllerGetAxis(
          this->gamepads[instanceId].gameController,
          static_cast<SDL_GameControllerAxis>(analogKey)) > 0;
}

bool SDLPlatformPlugin::GetButtonState(int32_t instanceId, int32_t buttonId) noexcept {
  return this->gamepads.contains(instanceId)
      && SDL2::SDL_JoystickGetButton(this->gamepads[instanceId].joystick, buttonId) > 0;
}

float SDLPlatformPlugin::GetAxisState(int32_t instanceId, int32_t axis) noexcept {
  if (this->gamepads.contains(instanceId)) {
    return AxisValueToFloat(SDL2::SDL_JoystickGetAxis(this->gamepads[instanceId].joystick, axis));
  }

  return 0;
}

int32_t SDLPlatformPlugin::GetHatState(int32_t instanceId, int32_t hat) noexcept {
  if (this->gamepads.contains(instanceId)) {
    return SDL2::SDL_JoystickGetHat(this->gamepads[instanceId].joystick, hat);
  }

  return SDL_HAT_CENTERED;
}

GamepadInfo SDLPlatformPlugin::GetGamepadInfo(int32_t instanceId) {
  if (!this->gamepads.contains(instanceId)) {
    return { -1 };
  }

  const auto& gamepad{ this->gamepads[instanceId] };
  const char* joystickName;

  if (gamepad.gameController) {
    joystickName = SDL2::SDL_GameControllerName(gamepad.gameController);
  } else {
    joystickName = SDL2::SDL_JoystickName(gamepad.joystick);
  }

  if (joystickName == nullptr) {
    LOG_ERROR("SDL_JoystickName(%i): %s", instanceId, SDL2::SDL_GetError());
  }

  auto joystickGUID{ SDL2::SDL_JoystickGetGUID(gamepad.joystick) };
  char guid[33];

  guid[0] = '\0';
  SDL2::SDL_JoystickGetGUIDString(joystickGUID, guid, 33);

  auto buttonCount = SDL2::SDL_JoystickNumButtons(gamepad.joystick);

  if (buttonCount < 0) {
    LOG_ERROR("SDL_JoystickNumButtons(%i): %s", instanceId, SDL2::SDL_GetError());
    buttonCount = 0;
  }

  auto axisCount = SDL2::SDL_JoystickNumAxes(gamepad.joystick);

  if (axisCount < 0) {
    LOG_ERROR("SDL_JoystickNumAxes(%i): %s", instanceId, SDL2::SDL_GetError());
    axisCount = 0;
  }

  auto hatCount = SDL2::SDL_JoystickNumHats(gamepad.joystick);

  if (hatCount < 0) {
    LOG_ERROR("SDL_JoystickNumHats(%i): %s", instanceId, SDL2::SDL_GetError());
    hatCount = 0;
  }

  GamepadInfo gamepadInfo {
    instanceId,
    joystickName,
    buttonCount,
    hatCount,
    axisCount
  };

  if (guid[0]) {
    memccpy(gamepadInfo.uuid, guid, 1, 32);
  }

  return gamepadInfo;
}

void SDLPlatformPlugin::DispatchGamepadConnected(int32_t index) {
  auto instanceId{ this->AddGamepadByIndex(index) };

  if (instanceId >= 0) {
    this->onGamepadStatus(instanceId, true);
  }
}

void SDLPlatformPlugin::DispatchGamepadDisconnected(int32_t instanceId) {
  if (this->RemoveGamepad(instanceId)) {
    this->onGamepadStatus(instanceId, false);
  }
}

int32_t SDLPlatformPlugin::AddGamepadByIndex(int32_t index) {
  int32_t instanceId;

  if (SDL2::SDL_JoystickGetDeviceInstanceID) {
    instanceId = SDL2::SDL_JoystickGetDeviceInstanceID(index);
  } else {
    // For older versions of SDL, the only API to get the instance ID from an index
    // requires opening and closing a joystick. Since the joystick objects are ref
    // counted, the open call will not disturb any open joysticks.
    auto joystick{ SDL2::SDL_JoystickOpen(index) };

    if (joystick) {
      instanceId = SDL2::SDL_JoystickInstanceID(joystick);
      SDL2::SDL_JoystickClose(joystick);
    } else {
      instanceId = -1;
    }
  }

  if (instanceId < 0) {
    LOG_ERROR("No instance id for joystick index: %i", index);
    return -1;
  }

  if (this->gamepads.contains(instanceId)) {
    return -1;
  }

  GamepadInstance gamepad{};

  if (SDL2::SDL_IsGameController(index) == SDL_TRUE) {
    gamepad.gameController = SDL2::SDL_GameControllerOpen(index);

    if (!gamepad.gameController) {
      LOG_ERROR("SDL_GameControllerOpen(%i): %s", index, SDL2::SDL_GetError());
      return false;
    }

    gamepad.joystick = SDL2::SDL_GameControllerGetJoystick(gamepad.gameController);

    if (!gamepad.joystick) {
      SDL2::SDL_GameControllerClose(gamepad.gameController);
      LOG_ERROR("SDL_GameControllerGetJoystick(%i): %s", index, SDL2::SDL_GetError());
      return false;
    }
  } else {
    gamepad.joystick = SDL2::SDL_JoystickOpen(index);

    if (!gamepad.joystick) {
      LOG_ERROR(Format("SDL_JoystickOpen(%i): %s", index, SDL2::SDL_GetError()));
      return false;
    }
  }

  this->gamepads.insert_or_assign(instanceId, gamepad);

  return instanceId;
}

bool SDLPlatformPlugin::RemoveGamepad(int32_t instanceId) {
  if (!this->gamepads.contains(instanceId)) {
    return false;
  }

  const auto& gamepad = this->gamepads[instanceId];

  if (gamepad.gameController) {
    SDL2::SDL_GameControllerClose(gamepad.gameController);
    // note: do not close joystick, as it's lifecycle is owned by game controller.
  } else {
    SDL2::SDL_JoystickClose(gamepad.joystick);
  }

  this->gamepads.erase(instanceId);

  return true;
}

static float AxisValueToFloat(float value) noexcept {
  return value < 0 ? -(value / SDL_JOYSTICK_AXIS_MIN_F) : value / SDL_JOYSTICK_AXIS_MAX_F;
}

} // namespace lse
