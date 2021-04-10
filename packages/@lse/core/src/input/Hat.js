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
 * Flags for to help interpret the state of a gamepad hat (directional pad) value.
 *
 * @enum {number}
 * @readonly
 * @name module:@lse/core.core-enum.Hat
 */
export const Hat = Object.freeze({
  CENTERED: 0,
  UP: 0x01,
  RIGHT: 0x02,
  DOWN: 0x04,
  LEFT: 0x08
})
