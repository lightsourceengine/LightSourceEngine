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

import { ReactReconciler } from '@lse/react/reconciler'
import { TextElement } from './TextElement.js'
import { BoxElement } from './BoxElement.js'
import { ImageElement } from './ImageElement.js'
import { performance } from 'perf_hooks'

const kText = 'text'

const kElementNameToElementClass = {
  box: BoxElement,
  img: ImageElement,
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

    prepareForCommit (containerInfo) {
      return null
    },

    prepareUpdate (wordElement, type, oldProps, newProps) {
      return true
    },

    resetAfterCommit (containerInfo) {
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
    },

    clearContainer ({ node }) {
      node.children.forEach(n => node.destroy(n))
    }
  })
}
