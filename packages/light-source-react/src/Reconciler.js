/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { ReactReconciler } from 'light-source-reconciler'
import { TextElement } from './TextElement'
import { BoxElement } from './BoxElement'
import { ImageElement } from './ImageElement'
import { LinkElement } from './LinkElement'
import { performance } from 'perf_hooks'

const kText = 'text'

const kElementNameToElementClass = {
  box: BoxElement,
  div: BoxElement,
  img: ImageElement,
  link: LinkElement,
  [kText]: TextElement
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

export const Reconciler = (scene) => {
  return ReactReconciler({
    now: performance.now,
    supportsMutation: true,
    supportsHydration: false,
    supportsPersistence: false,
    isPrimaryRenderer: true,

    appendInitialChild: appendChild,

    createInstance (type, props, rootContainerInstance, _currentHostContext, workInProgress) {
      return new (kElementNameToElementClass[type] || errorUnknownElementType(type))(scene.createNode(type), props)
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
      return type === kText
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
      // noop
    },

    shouldDeprioritizeSubtree (type, nextProps) {
      // noop
    }
  })
}
