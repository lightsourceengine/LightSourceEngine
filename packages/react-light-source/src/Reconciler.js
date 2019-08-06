/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import './performance-polyfill'
import {
  unstable_scheduleCallback as scheduleCallback,
  unstable_cancelCallback as cancelCallback,
  unstable_shouldYield as shouldYield
} from 'scheduler'
import ReactFiberReconciler from 'react-reconciler'
import { TextElement } from './TextElement'
import { BoxElement } from './BoxElement'
import { ImageElement } from './ImageElement'

const TEXT = 'text'
const BOX = 'box'
const DIV = 'div'
const IMG = 'img'

const ELEMENTS = {
  [TEXT]: TextElement,
  [BOX]: BoxElement,
  [DIV]: BoxElement,
  [IMG]: ImageElement
}

const appendChild = (parentInstance, child) => {
  parentInstance.appendChild(child)
}

const removeChild = (parentInstance, child) => {
  parentInstance.removeChild(child)
}

const errorUnknownElementType = (type) => {
  throw Error(`Invalid react element type '${type}'.`)
}

export function Reconciler (scene) {
  return ReactFiberReconciler({
    now: global.performance.now,

    supportsMutation: true,

    isPrimaryRenderer: true,

    appendInitialChild: appendChild,

    createInstance (type, props, rootContainerInstance, _currentHostContext, workInProgress) {
      return new (ELEMENTS[type] || errorUnknownElementType(type))(scene.createNode(type), props)
    },

    createTextInstance (text, rootContainerInstance, internalInstanceHandle) {
      return text
    },

    finalizeInitialChildren (wordElement, type, props) {
      return false
    },

    getPublicInstance (inst) {
      return inst
    },

    prepareForCommit () {
      // noop
    },

    prepareUpdate (wordElement, type, oldProps, newProps) {
      return true
    },

    resetAfterCommit () {
      // noop
    },

    resetTextContent (wordElement) {
      // noop
    },

    getRootHostContext (instance) {
      return instance
    },

    getChildHostContext (instance) {
      return instance
    },

    shouldSetTextContent (type, props) {
      return type === TEXT
    },

    appendChild,

    appendChildToContainer: appendChild,

    removeChild,

    removeChildFromContainer: removeChild,

    insertBefore (parentInstance, child, beforeChild) {
      parentInstance.insertBefore(child, beforeChild)
    },

    commitUpdate (instance, updatePayload, type, oldProps, newProps) {
      if (instance.updateProps && oldProps !== newProps) {
        instance.updateProps(oldProps, newProps)
      }
    },

    commitMount (instance, updatePayload, type, oldProps, newProps) {
      // noop
    },

    commitTextUpdate (textInstance, oldText, newText) {

    },

    schedulePassiveEffects: scheduleCallback,

    cancelPassiveEffects: cancelCallback,

    shouldDeprioritizeSubtree (type, nextProps) {

    },

    scheduleDeferredCallback: scheduleCallback,

    cancelDeferredCallback: cancelCallback,

    shouldYield
  })
}
