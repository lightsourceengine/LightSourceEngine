// Light Source Engine Version 1.7.0
// Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

import { Style } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import { useRef, useEffect } from 'react';

import { jsx } from '@lse/react/jsx-runtime';

const sheet = Style.createStyleSheet({
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
});

const spritesheet = 'resource/universal-lpc-sprite_male_01_full.png';

const frameCount = [ 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 6, 6, 6, 6, 13, 13, 13, 13, 6 ];

const millisPerFrame = 1e3 / 9;

const AnimatedSprite = ({spriteSheetRow}) => {
  const ref = useRef(null);
  useEffect(() => {
    const {node} = ref.current;
    const {scene} = node;
    let currentFrame = 0;
    let millisUntilNextFrame = millisPerFrame;
    node.style.objectPositionY = spriteSheetRow * -64;
    const onAnimationFrame = (tick, lastTick) => {
      millisUntilNextFrame -= tick - lastTick;
      if (millisUntilNextFrame <= 0) {
        if (++currentFrame >= frameCount[spriteSheetRow]) {
          currentFrame = 0;
        }
        millisUntilNextFrame = millisPerFrame;
        node.style.objectPositionX = currentFrame * -64;
      }
      scene.requestAnimationFrame(onAnimationFrame);
    };
    scene.requestAnimationFrame(onAnimationFrame);
  }, []);
  return jsx('img', {
    ref,
    src: spritesheet,
    class: sheet.sprite
  });
};

const SpritesApp = () => {
  const sprites = frameCount.map((value, index) => jsx(AnimatedSprite, {
    spriteSheetRow: index
  }, index.toString()));
  return jsx('box', {
    class: sheet.body,
    children: sprites
  });
};

letThereBeLight(jsx(SpritesApp, {}), {
  scene: {
    fullscreen: false
  }
});
