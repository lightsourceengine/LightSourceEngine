/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { loadPluginById, PluginId, createRefGraphicsContext } from './index.js'
import { PluginType } from './PluginType.js'

export const loadPlugin = (id, options) => {
  if (id === PluginId.REF) {
    return loadRefPlatformPlugin()
  } else {
    return loadPluginById(id, options)
  }
}

const loadRefPlatformPlugin = () => {
  let attached = true

  return {
    id: PluginId.REF,
    type: PluginType.PLATFORM,
    attach() {
      attached = true
    },
    detach() {
      attached = false
    },
    destroy() {
      attached = false
    },
    isAttached() {
      return attached
    },
    getDisplays() {
      return [{
        id: 0,
        name: 'Test Display',
        defaultMode: { width: 1280, height: 720 },
        modes: [{ width: 1280, height: 720 }]
      }]
    },
    getVideoDriverNames() {
      return ['Test Video Driver']
    },
    createGraphicsContext (options) {
      return createRefGraphicsContext(options)
    },
    processEvents() {
      return true
    },
    getScanCodeState(scanCode) {
      return false
    },
    getGameControllerMapping(uuid) {
    },
    getGamepadInstanceIds() {
      return []
    },
    loadGameControllerMappings(file) {
      return 0
    },
    isKeyDown(id, key) {
      return false
    },
    getAnalogValue(id, analogKey) {
      return false
    },
    getButtonState(id, button) {
      return false
    },
    getAxisState(id, axis) {
      return false
    },
    getHatState(id, hat) {
      return false
    },
    getGamepadInfo(id) {
      return {id, type: 'gamepad'}
    },
    resetCallbacks() {
      for (const key of Object.keys(this)) {
        if (key.startsWith('on')) {
          this[key] = undefined
        }
      }
    }
  }
}
