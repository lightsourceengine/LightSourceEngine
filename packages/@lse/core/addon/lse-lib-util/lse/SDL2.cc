/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/SDL2.h>
#include <lse/internal/SharedLibrary.h>
#include <lse/Config.h>
#include <lse/System.h>
#include <std17/filesystem>
#include <lse/Log.h>

#define DYNAMIC_SDL_FUNCTION_PTR(NAME) decltype(&::NAME) NAME{};
#define DYNAMIC_LOAD_SDL_FUNCTION(NAME) \
    lse::SDL2::NAME = reinterpret_cast<decltype(&::NAME)>(sLibrary.GetSymbol(#NAME, true));
#define DYNAMIC_LOAD_OPTIONAL_SDL_FUNCTION(NAME) \
    lse::SDL2::NAME = reinterpret_cast<decltype(&::NAME)>(sLibrary.GetSymbol(#NAME, false));

namespace lse {
namespace SDL2 {

static internal::SharedLibrary sLibrary{};
DYNAMIC_FOR_EACH_SDL_FUNCTION(DYNAMIC_SDL_FUNCTION_PTR)
DYNAMIC_FOR_EACH_OPTIONAL_SDL_FUNCTION(DYNAMIC_SDL_FUNCTION_PTR)

void Open() {
  if (sLibrary.IsOpen()) {
    sLibrary.Ref();
    return;
  }

  if (kIsMac && !EnvEquals(kEnvSdlUseDylib, "1")) {
    std17::filesystem::path p(GetEnvOrDefault(kEnvRuntimeFrameworkPath, kDefaultRuntimeFrameworkPath));

    sLibrary.Open(p.append(kSDLFramework).append(kSDLFrameworkLib).c_str());
  } else {
    sLibrary.Open(GetEnvOrDefault(kEnvSdlLibName, kSDLDefaultLibName));
  }

  DYNAMIC_FOR_EACH_SDL_FUNCTION(DYNAMIC_LOAD_SDL_FUNCTION)
  DYNAMIC_FOR_EACH_OPTIONAL_SDL_FUNCTION(DYNAMIC_LOAD_OPTIONAL_SDL_FUNCTION)

  LOG_INFO("Successfully loaded SDL2 API");
}

void Close() noexcept {
  sLibrary.Close();
}

} // namespace SDL2
} // namespace lse

#undef DYNAMIC_LOAD_SDL_FUNCTION
#undef DYNAMIC_LOAD_OPTIONAL_SDL_FUNCTION
#undef DYNAMIC_SDL_FUNCTION_PTR
