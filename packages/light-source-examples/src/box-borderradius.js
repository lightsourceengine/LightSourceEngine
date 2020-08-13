/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage, absoluteFill } from 'light-source'
import { renderer } from 'light-source-react'
import React from 'react'

// Demonstrates borderRadius style property on div/box elements.
const BorderRadiusApp = () => (
  <div style={{ padding: '10vh', flexDirection: 'row', flexWrap: 'wrap', justifyContent: 'space-between', backgroundColor: '#264653', ...absoluteFill }}>
    <div
      style={{ width: '30vh', height: '30vh', backgroundColor: '#2a9d8f', borderRadius: '3vh' }}
    />
    <div
      style={{ width: '30vh', height: '30vh', backgroundColor: '#e9c46a', borderRadiusTopLeft: '3vh' }}
    />
    <div
      style={{ width: '30vh', height: '30vh', backgroundColor: '#f4a261', borderRadiusTopRight: '3vh' }}
    />
    <div
      style={{ width: '30vh', height: '30vh', backgroundColor: '#e76f51', borderRadiusBottomLeft: '3vh' }}
    />
    <div
      style={{ width: '30vh', height: '30vh', backgroundColor: '#2a9d8f', borderRadiusBottomRight: '3vh' }}
    />
  </div>
)

const scene = stage.createScene({ fullscreen: false })

renderer(scene).render(<BorderRadiusApp />)

stage.start()
