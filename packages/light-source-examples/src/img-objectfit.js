/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { createStyleSheet } from 'light-source'
import { letThereBeLight } from 'light-source-react'
import React from 'react'

// Demonstrates objectFit on img elements.

const background = 'resource/pexels-emiliano-arano-1295138.jpg'

const sheet = createStyleSheet({
  body: {
    flexWrap: 'wrap',
    flexDirection: 'row',
    justifyContent: 'space-between',
    padding: '10vh',
    backgroundImage: background,
    '@extend': '%absoluteFill'
  },
  objectFitContain: {
    objectFit: 'contain',
    '@extend': '%cell'
  },
  objectFitFill: {
    objectFit: 'fill',
    '@extend': '%cell'
  },
  objectFitCover: {
    objectFit: 'cover',
    '@extend': '%cell'
  },
  objectFitScaleDown: {
    objectFit: 'scale-down',
    '@extend': '%cell'
  },
  objectFitNone: {
    objectFit: 'none',
    '@extend': '%cell'
  },
  '%cell': {
    border: '1vh',
    borderColor: 'red',
    '@size': '30vh'
  }
})

const ObjectFitApp = () => (
  <box class={sheet.body}>
    <img src={background} class={sheet.objectFitContain} />
    <img src={background} class={sheet.objectFitFill} />
    <img src={background} class={sheet.objectFitCover} />
    <img src={background} class={sheet.objectFitScaleDown} />
    <img src={background} class={sheet.objectFitNone} />
  </box>
)

letThereBeLight(<ObjectFitApp />, { fullscreen: false })
