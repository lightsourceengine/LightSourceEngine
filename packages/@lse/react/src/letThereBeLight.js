/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage, EventName } from '@lse/core'
import { render } from './renderer.js'

/**
 * Bootstraps a Light Source Engine application using React.
 *
 * Wraps up the process of initializing the stage, starting the main loop, creating a new Scene and
 * rendering a React element into the Scene. If more control is needed during the start up process,
 * all of these steps can be done by the user manually.
 *
 * If additional set up of Stage or managers (input, audio, etc) is needed, lifecycle methods are available on these
 * objects (even if they are not initialized) that can be used for configuration.
 *
 * @param element {Object} React element to render into the new scene
 * @param options ...
 * @returns {Scene} The application scene
 */
export const letThereBeLight = (element, options = {}) => {
  if (!stage.isConfigured()) {
    stage.configure(options)
  }

  const scene = stage.$scene || stage.createScene()

  // XXX: temporary fix so react nodes are cleaned up on exit; need to have render better handle the lifecycle of a container
  scene.once(EventName.onDestroying, () => render(scene, null))

  render(scene, element, options.callback)

  return scene
}
