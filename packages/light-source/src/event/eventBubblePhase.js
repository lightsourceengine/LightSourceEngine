/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { BUBBLE } from './EventPhase'

const bubble = (stage, scene, event) => {
  const callbackProperty = 'on' + event.name
  let walker = scene.activeNode
  let callback

  while (walker && !event.cancelled) {
    (callback = walker[callbackProperty]) && callback(event)
    walker = walker.parent
  }

  event.cancelled || scene.$emit(event)
  event.cancelled || stage.$emit(event)
}

/**
 * @ignore
 */
export const eventBubblePhase = (stage, scene, event) => {
  if (event.cancelled || !scene) {
    return
  }

  event.phase = BUBBLE
  bubble(stage, scene, event)
}
