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

/**
 * @enum {string}
 * @readonly
 * @name module:@lse/core.core-enum.PluginId
 */
export const PluginId = Object.freeze({
  SDL: "plugin:sdl",
  REF: "plugin:ref",
  SDL_AUDIO: "plugin:sdl-audio",
  SDL_MIXER: "plugin:sdl-mixer",
  REF_AUDIO: "plugin:ref-audio",
  NULL: "plugin:null"
})
