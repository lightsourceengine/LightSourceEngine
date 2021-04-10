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

namespace lse {

constexpr bool kEnablePluginPlatformSdl
#if defined(LSE_ENABLE_PLUGIN_PLATFORM_SDL)
    = true;
#else
= false;
#endif

constexpr bool kEnablePluginAudioSdlAudio
#if defined(LSE_ENABLE_PLUGIN_AUDIO_SDL_AUDIO)
    = true;
#else
= false;
#endif

constexpr bool kEnablePluginAudioSdlMixer
#if defined(LSE_ENABLE_PLUGIN_AUDIO_SDL_MIXER)
    = true;
#else
= false;
#endif

constexpr const char* kDefaultRuntimeFrameworkPath = "/Library/Frameworks";

} // namespace lse
