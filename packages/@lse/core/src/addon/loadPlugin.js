/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { loadRefPlugin, loadSDLPlugin, loadSDLAudioPlugin, loadSDLMixerPlugin } from './index.js'
import { PluginType } from './PluginType.js'
import { PluginId } from './PluginId.js'

export const loadPlugin = (id, options) => {
  switch (id) {
    case PluginId.SDL:
      return loadSDLPlugin(options)
    case PluginId.SDL_AUDIO:
      return loadSDLAudioPlugin(options)
    case PluginId.SDL_MIXER:
      return loadSDLMixerPlugin(options)
    case PluginId.REF:
      return loadRefPluginJs()
    case PluginId.REF_AUDIO:
      return loadRefAudioPluginJs()
    default:
      throw Error(`Unsupported plugin: ${id}`)
  }
}

const loadRefPluginJs = () => {
  let attached = true

  const GraphicsContext = loadRefPlugin()

  return {
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
      return new GraphicsContext(options)
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

const loadRefAudioPluginJs = () => {
  let attached

  class RefAudioDestination {
    decoders = [ "WAVE" ]
    volume = 0
    createAudioSource() {
      return {
        volume: 0,
        load() {
        },
        destroy() {
        },
        play() {
        },
        hasCapability() {
          return true
        }
      }
    }
    hasCapability() { return true }
    destroy() {}
    resume() {}
    pause() {}
    stop() {}
  }

  return {
    type: PluginType.AUDIO,
    devices: [ "Reference" ],
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
    createSampleAudioDestination() {
      return new RefAudioDestination()
    },
    createStreamAudioDestination() {
      return new RefAudioDestination()
    }
  }
}
