/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { SceneBase, BoxSceneNode, ImageSceneNode, TextSceneNode } from '../addon'
import { Style } from '../style/Style'
import { EventEmitter } from '../util/EventEmitter'
import { EventType } from '../event/EventType'
import { navigationManager } from './navigationManager'
import {
  $width,
  $height,
  $fullscreen,
  $stage,
  $root,
  $resource,
  $destroy,
  $displayIndex,
  $activeNode,
  $destroying,
  $capture,
  $bubble,
  $events,
  $navigationManager,
  $setActiveNode,
  $hasFocus,
  $resourcePath
} from '../util/InternalSymbols'
import { BlurEvent } from '../event/BlurEvent'
import { FocusEvent } from '../event/FocusEvent'
import { performance } from 'perf_hooks'

const { now } = performance

export class Scene extends SceneBase {
  constructor (stage, stageAdapter, displayIndex, width, height, fullscreen) {
    super(stageAdapter, displayIndex, width, height, fullscreen)

    this[$stage] = stage
    this[$displayIndex] = displayIndex
    this[$events] = new EventEmitter([
      $destroying,
      EventType.KeyDown,
      EventType.KeyUp,
      EventType.AxisMotion,
      EventType.DeviceConnected,
      EventType.DeviceDisconnected,
      EventType.DeviceButtonDown,
      EventType.DeviceButtonUp,
      EventType.DeviceAxisMotion
    ])
    this[$activeNode] = null
    this[$navigationManager] = navigationManager
    this[$resource][$resourcePath] = stage.resourcePath

    this.root.style = new Style({
      position: 'absolute',
      left: 0,
      top: 0,
      right: 0,
      bottom: 0,
      fontSize: 16,
      backgroundColor: 'black'
    })
  }

  get stage () {
    return this[$stage]
  }

  get root () {
    return this[$root]
  }

  get resource () {
    return this[$resource]
  }

  get fullscreen () {
    return this[$fullscreen]
  }

  get width () {
    return this[$width]
  }

  get height () {
    return this[$height]
  }

  get displayIndex () {
    return this[$displayIndex]
  }

  on (id, listener) {
    this[$events].on(id, listener)
  }

  off (id, listener) {
    this[$events].off(id, listener)
  }

  get activeNode () {
    return this[$activeNode]
  }

  // TODO: add refreshRate

  // TODO: add vsync

  createNode (tag) {
    return new (nodeClass.get(tag) || throwNodeClassNotFound(tag))(this)
  }

  resize (width = 0, height = 0, fullscreen = true) {
    super.resize(width, height, fullscreen)
  }

  [$destroy] () {
    if (this[$events]) {
      this[$events].emit({ type: $destroying })
      this[$events] = null
    }

    this[$stage] = null
    this[$activeNode] = null
    this[$navigationManager] = null
    super[$destroy]()
  }

  [$capture] (event) {
    const activeNode = this[$activeNode]

    if (!activeNode || event.cancelled) {
      return
    }

    this[$navigationManager](this, activeNode, event)
  }

  [$bubble] (event) {
    const activeNode = this[$activeNode]

    if (!activeNode || event.cancelled) {
      return
    }

    const callbackProperty = eventToCallbackProperty.get(event.type)

    if (!callbackProperty) {
      throw ErrorUnsupportedEvent(event)
    }

    traverseAncestors(activeNode, node => {
      node[callbackProperty] && node[callbackProperty](event)

      return !event.cancelled
    })

    event.cancelled || this[$events].emit(event)
    event.cancelled || this.stage[$events].emit(event)
  }

  [$setActiveNode] (node) {
    // TODO: validate node

    if (this[$activeNode] === node) { // TODO: null === undefined
      return
    }

    const timestamp = now()

    if (this[$activeNode]) {
      traverseAncestors(this[$activeNode], clearHasFocus)

      const blurEvent = new BlurEvent(timestamp)

      traverseAncestors(this[$activeNode], node => {
        node.onBlur && node.onBlur(blurEvent)

        return !blurEvent.cancelled
      })

      blurEvent.cancelled || this[$events].emit(blurEvent)
      blurEvent.cancelled || this.stage[$events].emit(blurEvent)
    }

    this[$activeNode] = node

    if (this[$activeNode]) {
      traverseAncestors(this[$activeNode], setHasFocus)

      const focusEvent = new FocusEvent(timestamp)

      traverseAncestors(this[$activeNode], node => {
        node.onFocus && node.onFocus(focusEvent)

        return !focusEvent.cancelled
      })

      focusEvent.cancelled || this[$events].emit(focusEvent)
      focusEvent.cancelled || this.stage[$events].emit(focusEvent)
    }
  }
}

const nodeClass = new Map([
  ['img', ImageSceneNode],
  ['div', BoxSceneNode],
  ['box', BoxSceneNode],
  ['text', TextSceneNode]
])

const eventToCallbackProperty = new Map([
  [EventType.KeyUp, 'onKeyUp'],
  [EventType.KeyDown, 'onKeyDown'],
  [EventType.AxisMotion, 'onAxisMotion'],
  [EventType.DeviceButtonUp, 'onDeviceButtonUp'],
  [EventType.DeviceButtonDown, 'onDeviceButtonDown'],
  [EventType.DeviceAxisMotion, 'onDeviceAxisMotion'],
  [EventType.Focus, 'onFocus'],
  [EventType.Blur, 'onBlur']
])

const throwNodeClassNotFound = tag => {
  throw new Error(`'${tag}' is not a valid scene node tag.`)
}

const ErrorUnsupportedEvent = event => Error(`Event ${event.name} unsupported by Scene.`)

const setHasFocus = node => {
  node[$hasFocus] = true
  node.waypoint && node.waypoint.sync(node)

  return true
}

const clearHasFocus = node => {
  node[$hasFocus] = false
  node.waypoint && node.waypoint.sync(node)

  return true
}

const traverseAncestors = (node, func) => {
  while (node) {
    if (!func(node)) {
      break
    }

    node = node.parent
  }
}
