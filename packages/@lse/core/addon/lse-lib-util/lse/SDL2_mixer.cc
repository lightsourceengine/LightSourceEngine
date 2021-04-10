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

#include <lse/SDL2_mixer.h>
#include <lse/internal//SharedLibrary.h>
#include <lse/Log.h>
#include <lse/System.h>
#include <lse/Config.h>
#include <std17/filesystem>

#define DYNAMIC_MIX_FUNCTION_PTR(NAME) decltype(&::NAME) NAME{};
#define DYNAMIC_LOAD_MIX_FUNCTION(NAME) \
    lse::SDL2::mixer::NAME = reinterpret_cast<decltype(&::NAME)>(sLibrary.GetSymbol(#NAME));

namespace lse {
namespace SDL2 {
namespace mixer {

static internal::SharedLibrary sLibrary{};
DYNAMIC_FOR_EACH_MIX_FUNCTION(DYNAMIC_MIX_FUNCTION_PTR)

void Open() {
  if (sLibrary.IsOpen()) {
    sLibrary.Ref();
    return;
  }

  if (kIsMac && !EnvEquals(kEnvSdlMixerUseDylib, "1")) {
    std17::filesystem::path p(GetEnvOrDefault(kEnvRuntimeFrameworkPath, kDefaultRuntimeFrameworkPath));

    sLibrary.Open(p.append(kSDLMixerFramework).append(kSDLMixerFrameworkLib).c_str());
  } else {
    sLibrary.Open(GetEnvOrDefault(kEnvSdlMixerLibName, kSDLMixerDefaultLibName));
  }

  DYNAMIC_FOR_EACH_MIX_FUNCTION(DYNAMIC_LOAD_MIX_FUNCTION)

  LOG_INFO("Successfully loaded SDL2 Mixer API");
}

void Close() noexcept {
  sLibrary.Close();
}

} // namespace mixer
} // namespace SDL2
} // namespace lse

#undef DYNAMIC_LOAD_MIX_FUNCTION
#undef DYNAMIC_MIX_FUNCTION_PTR
