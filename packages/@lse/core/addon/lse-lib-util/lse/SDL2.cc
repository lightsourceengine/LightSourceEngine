/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/SDL2.h>
#include <lse/internal/SharedLibrary.h>

#define DYNAMIC_SDL_FUNCTION_PTR(NAME) decltype(&::NAME) NAME{};
#define DYNAMIC_LOAD_SDL_FUNCTION(NAME) \
    lse::SDL2::NAME = reinterpret_cast<decltype(&::NAME)>(sLibrary.GetSymbol(#NAME));

namespace lse {
namespace SDL2 {

static internal::SharedLibrary sLibrary{};
DYNAMIC_FOR_EACH_SDL_FUNCTION(DYNAMIC_SDL_FUNCTION_PTR)

void Open(const char* library) {
    sLibrary.Open(library);

    DYNAMIC_FOR_EACH_SDL_FUNCTION(DYNAMIC_LOAD_SDL_FUNCTION)
}

bool IsOpen() noexcept {
    return sLibrary.IsOpen();
}

void Close() noexcept {
    sLibrary.Close();
}

} // namespace SDL2
} // namespace lse

#undef DYNAMIC_LOAD_SDL_FUNCTION
#undef DYNAMIC_SDL_FUNCTION_PTR
