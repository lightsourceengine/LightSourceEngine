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
 * @param options ...
 * @returns {module:@lse/core.Scene} The application scene
 * @method module:@lse/react.letThereBeLight
 */
export const letThereBeLight = (element, options = {}) => {
  if (!stage.isConfigured()) {
    stage.configure(options)
  }

  const scene = stage.$scene || stage.createScene()

  // XXX: temporary fix so react nodes are cleaned up on exit; need to have render better handle the lifecycle of a container
  scene.once('destroying', () => render(scene, null))

  render(scene, element, options.callback)

  return scene
}
