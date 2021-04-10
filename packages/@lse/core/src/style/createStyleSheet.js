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

import { createStyleInStyleSheet } from './createStyleClass.js'

/**
 * Create a new style sheet.
 *
 * Each style spec is converted to a Style using createStyle(). The same rules apply.
 *
 * When creating a style sheet, local mixins are supported. If a spec key starts with '%', it is treated as a local
 * mixin. A Style will not be generated or returned, but the mixin can be referenced by other specs in this style
 * sheet.
 *
 * @param {Object} spec Object containing style names as keys and style property specs as values. specs are just
 * plain Objects with style properties
 * @method module:@lse/core.createStyleSheet
 */
export const createStyleSheet = (spec) => {
  if (!spec || typeof spec !== 'object') {
    throw Error(`Invalid createStyleSheet argument: ${spec}`)
  }

  const result = {}

  for (const key in spec) {
    // skip mixins
    if (key.startsWith('%')) {
      continue
    }

    // name that start with @ are reserved by createStyleSheet
    if (key.startsWith('@')) {
      throw Error('style name cannot be prefixed with @')
    }

    const entry = spec[key]

    if (entry) {
      // Object -> Style (createStyleInStyleSheet will handle @extend)
      result[key] = createStyleInStyleSheet(entry, key, spec)
    } else {
      throw Error(`invalid style sheet entry at ${key}`)
    }
  }

  return result
}
