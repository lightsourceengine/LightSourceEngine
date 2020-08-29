/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { createStyleSheet } from 'light-source'
import { letThereBeLight } from 'light-source-react'
import React from 'react'

// Demonstrates borderRadius style property on box elements.

const sheet = createStyleSheet({
  // color palette: https://coolors.co/264653-2a9d8f-e9c46a-f4a261-e76f51
  body: {
    padding: '10vh',
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'space-between',
    backgroundColor: '#264653',
    '@extend': '%absoluteFill'
  },
  borderRadius: {
    backgroundColor: '#2a9d8f',
    borderRadius: '3vh',
    '@extend': '%size'
  },
  borderRadiusTopLeft: {
    backgroundColor: '#e9c46a',
    borderRadiusTopLeft: '3vh',
    '@extend': '%size'
  },
  borderRadiusTopRight: {
    backgroundColor: '#f4a261',
    borderRadiusTopRight: '3vh',
    '@extend': '%size'
  },
  borderRadiusBottomLeft: {
    backgroundColor: '#e76f51',
    borderRadiusBottomLeft: '3vh',
    '@extend': '%size'
  },
  borderRadiusBottomRight: {
    backgroundColor: '#2a9d8f',
    borderRadiusBottomRight: '3vh',
    '@extend': '%size'
  },
  '%size': {
    '@size': '30vh'
  }
})

const BorderRadiusApp = () => (
  <box style={sheet.body}>
    <box style={sheet.borderRadius} />
    <box style={sheet.borderRadiusTopLeft} />
    <box style={sheet.borderRadiusTopRight} />
    <box style={sheet.borderRadiusBottomLeft} />
    <box style={sheet.borderRadiusBottomRight} />
  </box>
)

letThereBeLight(<BorderRadiusApp />, { fullscreen: false })
