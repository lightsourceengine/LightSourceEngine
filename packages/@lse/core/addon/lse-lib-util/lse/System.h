/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

namespace lse {

// Platform Identity API

#if defined(_WIN32)
constexpr const bool kIsWindows = true;
#else
constexpr const bool kIsWindows = false;
#endif

#if defined(__linux__)
constexpr const bool kIsLinux = true;
#else
constexpr const bool kIsLinux = false;
#endif

#if defined(__APPLE__)
constexpr const bool kIsMac = true;
#else
constexpr const bool kIsMac = false;
#endif

// Endianness API

#define LSE_LITTLE_ENDIAN 0
#define LSE_BIG_ENDIAN 1

#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
#define LSE_BYTE_ORDER LSE_BIG_ENDIAN
constexpr const bool kIsBigEndian = true;
constexpr const bool kIsLittleEndian = false;
#else
#define LSE_BYTE_ORDER LSE_LITTLE_ENDIAN
constexpr const bool kIsBigEndian = false;
constexpr const bool kIsLittleEndian = true;
#endif

// Environment Variable API

constexpr const char* kEnvSdlLibName = "LSE_SDL_LIB_NAME";
constexpr const char* kEnvRuntimeFrameworkPath = "LSE_RUNTIME_FRAMEWORK_PATH";
constexpr const char* kEnvSdlUseDylib = "LSE_SDL_USE_DYLIB";
constexpr const char* kEnvSdlMixerLibName = "LSE_SDL_MIXER_LIB_NAME";
constexpr const char* kEnvSdlMixerUseDylib = "LSE_SDL_MIXER_USE_DYLIB";

bool HasEnv(const char* name) noexcept;
const char* GetEnv(const char* name) noexcept;
const char* GetEnvOrDefault(const char* name, const char* defaultValue) noexcept;
bool EnvEquals(const char* env, const char* expectedValue) noexcept;

} // namespace lse
