/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage, absoluteFill } from 'light-source'
import { render } from 'light-source-react'
import React from 'react'

// Demonstrates backgroundRepeat style property on div/box elements.
const BackgroundRepeatApp = () => (
  <div style={{ backgroundImage: 'resource/pattern-1004855.jpg', backgroundRepeat: 'repeat', ...absoluteFill }} />
)

const scene = stage.createScene({ fullscreen: false })

render(scene, <BackgroundRepeatApp />)

stage.start()
