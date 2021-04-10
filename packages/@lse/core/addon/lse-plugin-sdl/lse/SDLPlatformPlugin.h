/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <vector>
#include <string>
#include <phmap.h>
#include <lse/SDL2.h>

namespace lse {

struct SDLDisplayMode {
  int32_t width{};
  int32_t height{};
};

struct SDLDisplay {
  int32_t id;
  std::string name;
  SDLDisplayMode defaultMode;
  std::vector<SDLDisplayMode> modes;
};

struct GamepadInfo {
  int32_t instanceId;
  const char* name{};
  int32_t buttonCount{};
  int32_t hatCount{};
  int32_t axisCount{};
  char uuid[33]{'\0'};
};

class SDLPlatformPlugin {
 public:
  SDLPlatformPlugin();

  void Attach();
  void Detach() noexcept;
  bool IsAttached() const noexcept { return this->isAttached; }
  void Destroy() noexcept;
  std::vector<std::string> GetVideoDriverNames();
  std::vector<SDLDisplay> GetDisplays();
  bool GetScanCodeState(int32_t scanCode) noexcept;
  std::vector<int32_t> GetGamepadInstanceIds();
  bool IsKeyDown(int32_t instanceId, int32_t key) noexcept;
  float GetAnalogValue(int32_t instanceId, int32_t analogValue) noexcept;
  bool GetButtonState(int32_t instanceId, int32_t buttonState) noexcept;
  float GetAxisState(int32_t instanceId, int32_t axis) noexcept;
  int32_t GetHatState(int32_t instanceId, int32_t hat) noexcept;
  GamepadInfo GetGamepadInfo(int32_t instanceId);

  int32_t LoadGameControllerMappings(const void* utf8, size_t len) noexcept;
  const char* GetGameControllerMapping(const char* uuid) noexcept;
  bool ProcessEvents() noexcept;

 public:
  void (*onKeyboardScanCode)(int32_t, bool, bool) noexcept {};
  void (*onGamepadStatus)(int32_t, bool) noexcept {};
  void (*onGamepadAxis)(int32_t, int32_t, float) noexcept {};
  void (*onGamepadAxisMapped)(int32_t, int32_t, float) noexcept {};
  void (*onGamepadHat)(int32_t, int32_t, int32_t) noexcept {};
  void (*onGamepadButton)(int32_t, int32_t, bool) noexcept {};
  void (*onGamepadButtonMapped)(int32_t, int32_t, bool) noexcept {};
  bool (*onQuit)() noexcept {};

 protected:
  int32_t AddGamepadByIndex(int32_t index);
  bool RemoveGamepad(int32_t instanceId);
  void DispatchGamepadConnected(int32_t index);
  void DispatchGamepadDisconnected(int32_t instanceId);

 private:
  struct GamepadInstance {
    int32_t instanceId{-1};
    SDL_Joystick* joystick{};
    SDL_GameController* gameController{};
  };

 private:
  bool isAttached{false};
  phmap::flat_hash_map<int32_t, GamepadInstance> gamepads;
};

} // namespace lse
