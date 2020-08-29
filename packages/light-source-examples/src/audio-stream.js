/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage, createStyleSheet, AudioDecoderType } from 'light-source'
import { letThereBeLight } from 'light-source-react'
import React, { useEffect, useState } from 'react'

// Demonstrates playing an mp3 or ogg file.

const sheet = createStyleSheet({
  body: {
    backgroundColor: '#8d99ae',
    padding: 20,
    '@extend': '%absoluteFill'
  },
  label: {
    fontFamily: 'Roboto',
    color: '#2b2d42',
    fontSize: 24
  }
})

const StreamingAudioApp = () => {
  const [loadingStatus, setLoadingStatus] = useState('loading')

  useEffect(() => {
    let path

    if (stage.audio.stream.hasDecoder(AudioDecoderType.OGG)) {
      path = 'resource/bensound-ukulele.ogg'
    } else if (stage.audio.stream.hasDecoder(AudioDecoderType.MP3)) {
      path = 'resource/bensound-ukulele.mp3'
    } else {
      setLoadingStatus('not ready')
      return
    }

    stage.audio.addStream(path).once('status', (event) => {
      if (event.target.isReady()) {
        event.target.play()
        setLoadingStatus('ready')
      } else {
        setLoadingStatus('not ready')
        console.log(event.error)
      }
    })
  })

  return (
    <box style={sheet.body}>
      <link href='file:resource/Roboto-Bold.ttf?family=Roboto' />
      <text style={sheet.label}>{`Background music is ${loadingStatus}.`}</text>
    </box>
  )
}

letThereBeLight(<StreamingAudioApp />, { fullscreen: false })
