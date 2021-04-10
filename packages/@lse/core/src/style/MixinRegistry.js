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

const kMixinMap = new Map()

/**
 * @memberof module:@lse/core
 * @hideconstructor
 */
class MixinRegistry {
  static add (id, spec) {
    if (!id || typeof id !== 'string' || !id.startsWith('%')) {
      throw Error(`id must be a string that starts with '%' (${id})`)
    }

    if (!spec || typeof spec !== 'object') {
      throw Error('expand must be a plain object')
    }

    if (kMixinMap.has(id)) {
      throw Error(`Mixin '${id}' already exists`)
    }

    kMixinMap.set(id, spec)
  }

  static get (id) {
    return kMixinMap.get(id)
  }

  static delete (id) {
    kMixinMap.delete(id)
  }

  static has (id) {
    return kMixinMap.has(id)
  }
}

export const resetMixinRegistry = () => {
  kMixinMap.clear()
  kMixinMap.set('%absoluteFill', {
    position: 'absolute',
    left: 0,
    top: 0,
    right: 0,
    bottom: 0
  })
}

resetMixinRegistry()

export { MixinRegistry }
