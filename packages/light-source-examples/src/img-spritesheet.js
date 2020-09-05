/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { createStyleSheet } from 'light-source'
import { letThereBeLight } from 'light-source-react'
import React, { useEffect, useRef } from 'react'

// Demonstrates objectPosition on img elements and manual animations.

const sheet = createStyleSheet({
  body: {
    padding: '10vh',
    flexWrap: 'wrap',
    flexDirection: 'row',
    justifyContent: 'flex-start',
    backgroundColor: 'lightgray',
    '@extend': '%absoluteFill'
  },
  sprite: {
    objectFit: 'none',
    marginBottom: 20,
    marginRight: 20,
    '@size': 64,
    '@objectPosition': 0
  }
})

// sprite sheet, containing 21 rows of animations. each animation is 64x64.
const spritesheet = 'resource/universal-lpc-sprite_male_01_full.png'
// array of number of frames of animation in each row of the sprite sheet.
const frameCount = [7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 6, 6, 6, 6, 13, 13, 13, 13, 6]
// run at 9 frames per second
const millisPerFrame = 1000 / 9

// Sprite animation component.
// Uses requestAnimationFrame() and objectPositionX/Y to do sprite animation.
const AnimatedSprite = ({ spriteSheetRow }) => {
  const ref = useRef(null)

  useEffect(() => {
    const { node } = ref.current
    const { scene } = node
    let currentFrame = 0
    let millisUntilNextFrame = millisPerFrame

    node.style.objectPositionY = spriteSheetRow * -64

    // Note, Light Source Engine does not have an animation API at the moment, but animations
    // are possible through rAF and manually changing styles.
    const onAnimationFrame = (tick, lastTick) => {
      millisUntilNextFrame -= (tick - lastTick)

      if (millisUntilNextFrame <= 0) {
        if (++currentFrame >= frameCount[spriteSheetRow]) {
          currentFrame = 0
        }

        millisUntilNextFrame = millisPerFrame
        node.style.objectPositionX = currentFrame * -64
      }

      scene.requestAnimationFrame(onAnimationFrame)
    }

    scene.requestAnimationFrame(onAnimationFrame)
  })

  return (<img ref={ref} src={spritesheet} class={sheet.sprite} />)
}

const SpritesApp = () => {
  const sprites = frameCount.map((value, index) => <AnimatedSprite spriteSheetRow={index} key={index.toString()} />)

  return (<box class={sheet.body}>{sprites}</box>)
}

letThereBeLight(<SpritesApp />, { fullscreen: false })
