/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage, absoluteFill } from 'light-source'
import { render } from 'light-source-react'
import React from 'react'

const label = {
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

const sampleText = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam dignissim at metus in finibus. Duis pharetra lobortis ultrices. Donec ullamcorper elementum ultricies.'
const sampleTextSmall = '   Hello, world!   '
const sampleTextNewline = '1) Eat\n2) Sleep\n3) Conquer\n4) Repeat'

// Demonstrates textAlign style property on text elements.
const TextAlignApp = () => (
  <div style={{ padding: '10vh', flexDirection: 'row', flexWrap: 'wrap', justifyContent: 'flex-start', backgroundColor: '#264653', ...absoluteFill }}>
    <link href='resource/Roboto-Bold.ttf' />
    <text style={{ textAlign: 'left', maxLines: 1, ...label }}>{sampleTextSmall}</text>
    <text style={{ textAlign: 'center', maxLines: 1, ...label }}>{sampleTextSmall}</text>
    <text style={{ textAlign: 'right', maxLines: 1, ...label }}>{sampleTextSmall}</text>
    <text style={{ textAlign: 'left', ...label }}>{sampleText}</text>
    <text style={{ textAlign: 'center', ...label }}>{sampleText}</text>
    <text style={{ textAlign: 'right', ...label }}>{sampleText}</text>
    <text style={{ textAlign: 'left', maxLines: 1, textOverflow: 'ellipsis', ...label }}>{sampleText}</text>
    <text style={{ textAlign: 'center', maxLines: 1, textOverflow: 'ellipsis', ...label }}>{sampleText}</text>
    <text style={{ textAlign: 'right', maxLines: 1, textOverflow: 'ellipsis', ...label }}>{sampleText}</text>
    <text style={{ textAlign: 'left', ...label }}>{sampleTextNewline}</text>
    <text style={{ textAlign: 'center', ...label }}>{sampleTextNewline}</text>
    <text style={{ textAlign: 'right', ...label }}>{sampleTextNewline}</text>
  </div>
)

const scene = stage.createScene({ fullscreen: false })

render(scene, <TextAlignApp />)

stage.start()
