/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { createStyleSheet } from 'light-source'
import { letThereBeLight } from 'light-source-react'
import React from 'react'

// Demonstrates textAlign style property on text elements.

const sheet = createStyleSheet({
  body: {
    padding: '10vh',
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'flex-start',
    backgroundColor: '#264653',
    '@extend': '%absoluteFill'
  },
  textAlignLeft: {
    textAlign: 'left',
    '@extend': '%label'
  },
  textAlignCenter: {
    textAlign: 'center',
    '@extend': '%label'
  },
  textAlignRight: {
    textAlign: 'right',
    '@extend': '%label'
  },
  ellipsisTextAlignLeft: {
    '@extend': ['textAlignLeft', '%ellipsis']
  },
  ellipsisTextAlignCenter: {
    '@extend': ['textAlignCenter', '%ellipsis']
  },
  ellipsisTextAlignRight: {
    '@extend': ['textAlignRight', '%ellipsis']
  },
  maxLinesTextAlignLeft: {
    maxLines: 1,
    '@extend': 'textAlignLeft'
  },
  maxLinesTextAlignCenter: {
    maxLines: 1,
    '@extend': 'textAlignCenter'
  },
  maxLinesTextAlignRight: {
    maxLines: 1,
    '@extend': 'textAlignRight'
  },
  '%ellipsis': {
    maxLines: 1,
    textOverflow: 'ellipsis'
  },
  '%label': {
    fontFamily: 'Roboto-Bold',
    fontSize: 24,
    color: 'lightgray',
    width: '50vh',
    border: 2,
    borderColor: 'white',
    padding: 20,
    marginRight: 10,
    marginBottom: 10
  }
})

const sampleText = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam dignissim at metus in finibus. Duis pharetra lobortis ultrices. Donec ullamcorper elementum ultricies.'
const sampleTextSmall = '   Hello, world!   '
const sampleTextNewline = '1) Eat\n2) Sleep\n3) Conquer\n4) Repeat'

const TextAlignApp = () => (
  <box class={sheet.body}>
    <link href='resource/Roboto-Bold.ttf' />
    <text class={sheet.maxLinesTextAlignLeft}>{sampleTextSmall}</text>
    <text class={sheet.maxLinesTextAlignCenter}>{sampleTextSmall}</text>
    <text class={sheet.maxLinesTextAlignRight}>{sampleTextSmall}</text>
    <text class={sheet.textAlignLeft}>{sampleText}</text>
    <text class={sheet.textAlignCenter}>{sampleText}</text>
    <text class={sheet.textAlignRight}>{sampleText}</text>
    <text class={sheet.ellipsisTextAlignLeft}>{sampleText}</text>
    <text class={sheet.ellipsisTextAlignCenter}>{sampleText}</text>
    <text class={sheet.ellipsisTextAlignRight}>{sampleText}</text>
    <text class={sheet.textAlignLeft}>{sampleTextNewline}</text>
    <text class={sheet.textAlignCenter}>{sampleTextNewline}</text>
    <text class={sheet.textAlignRight}>{sampleTextNewline}</text>
  </box>
)

letThereBeLight(<TextAlignApp />, { fullscreen: false })
