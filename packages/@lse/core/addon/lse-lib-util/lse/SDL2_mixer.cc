/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/SDL2_mixer.h>
#include <lse/internal//SharedLibrary.h>

#define DYNAMIC_MIX_FUNCTION_PTR(NAME) decltype(&::NAME) NAME{};
#define DYNAMIC_LOAD_MIX_FUNCTION(NAME) \
    lse::SDL2::mixer::NAME = reinterpret_cast<decltype(&::NAME)>(sLibrary.GetSymbol(#NAME));

namespace lse {
namespace SDL2 {
namespace mixer {

static internal::SharedLibrary sLibrary{};
DYNAMIC_FOR_EACH_MIX_FUNCTION(DYNAMIC_MIX_FUNCTION_PTR)

void Open(const char* library) {
  sLibrary.Open(library);
  DYNAMIC_FOR_EACH_MIX_FUNCTION(DYNAMIC_LOAD_MIX_FUNCTION)
}

bool IsOpen() noexcept {
  return sLibrary.IsOpen();
}

void Close() noexcept {
  sLibrary.Close();
}

} // namespace mixer
} // namespace SDL2
} // namespace lse

#undef DYNAMIC_LOAD_MIX_FUNCTION
#undef DYNAMIC_MIX_FUNCTION_PTR
