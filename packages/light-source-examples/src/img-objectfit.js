/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage } from 'light-source'
import { renderer } from 'light-source-react'
import React from 'react'

const background = 'resource/pexels-emiliano-arano-1295138.jpg'

// Demonstrates objectFit on img elements.
const ObjectFitApp = () => (
  <div style={{ padding: '10vh', height: '100%', backgroundImage: background }}>
    <div style={{ width: '100%', flexWrap: 'wrap', flexDirection: 'row', justifyContent: 'space-between' }}>
      <img
        src={background}
        style={{ width: '30vh', height: '30vh', border: 4, borderColor: 'red', objectFit: 'contain' }}
      />
      <img
        src={background}
        style={{ width: '30vh', height: '30vh', border: 4, borderColor: 'red', objectFit: 'fill' }}
      />
      <img
        src={background}
        style={{ width: '30vh', height: '30vh', border: 4, borderColor: 'red', objectFit: 'cover' }}
      />
      <img
        src={background}
        style={{ width: '30vh', height: '30vh', border: 4, borderColor: 'red', objectFit: 'scale-down' }}
      />
      <img
        src={background}
        style={{ width: '30vh', height: '30vh', border: 4, borderColor: 'red', objectFit: 'none' }}
      />
    </div>
  </div>
)

const scene = stage.createScene({ fullscreen: false })

renderer(scene).render(<ObjectFitApp />)

stage.start()
