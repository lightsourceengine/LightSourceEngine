/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { stage, absoluteFill, createStyleSheet, AudioDecoderType } from 'light-source'
import { render } from 'light-source-react'
import React, { useEffect, useState } from 'react'

const styles = createStyleSheet({
  body: {
    backgroundColor: 'dodgerblue',
    padding: 20,
    ...absoluteFill
  },
  label: {
    fontFamily: 'Roboto',
    color: 'white',
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
    <box style={styles.body}>
      <link href='file:resource/Roboto-Bold.ttf?family=Roboto' />
      <text style={styles.label}>{`Background music is ${loadingStatus}.`}</text>
    </box>
  )
}

const scene = stage.createScene({ fullscreen: false })

render(scene, <StreamingAudioApp />)

stage.start()
