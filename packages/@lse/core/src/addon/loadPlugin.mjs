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

import { loadRefPlugin, loadSDLPlugin, loadSDLAudioPlugin, loadSDLMixerPlugin } from './index.mjs'
import { PluginType } from './PluginType.mjs'
import { PluginId } from './PluginId.mjs'

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
    getDecoders() {
      return [ "WAVE" ]
    }
    getVolume() {
      return 0
    }
    setVolume(volume) {
    }
    createAudioSource() {
      return {
        getVolume() {
          return 0
        },
        setVolume(volume) {
        },
        load() {
        },
        unload() {
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
    getDevices() {
      return [ "Reference" ]
    },
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
