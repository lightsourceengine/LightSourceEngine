/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { style } from 'light-source'
import { letThereBeLight } from 'light-source-react'
import React from 'react'

// Demonstrates backgroundRepeat style property on div/box elements.

const body = style({
  backgroundImage: 'resource/pattern-1004855.jpg',
  backgroundRepeat: 'repeat',
  '@extend': '%absoluteFill'
})

letThereBeLight(<box style={body} />, { fullscreen: false })
