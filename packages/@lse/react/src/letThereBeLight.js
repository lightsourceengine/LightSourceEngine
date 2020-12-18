/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage } from '@lse/core'
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
 * @param sceneConfig {Object} Scene configuration. @see Stage.createScene()
 * @returns {Scene} The application scene
 */
export const letThereBeLight = (element, sceneConfig = {}) => {
  let scene

  stage.init()
  stage.start()
  render(scene = stage.createScene(sceneConfig), element)

  return scene
}
