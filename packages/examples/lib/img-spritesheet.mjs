/*
 * Light Source Engine
 * Copyright (C) 2019-2021 Daniel Anderson <dan.anderson.oss@gmail.com>.
 *
 * This source code is licensed under the MIT License.
 *
 * License Text:    https://github.com/lightsourceengine/LightSourceEngine/blob/master/LICENSE
 * Original Source: https://github.com/lightsourceengine/LightSourceEngine
 */

import { jsx } from '@lse/react/jsx-runtime';

import { Style } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import { useRef, useEffect } from 'react';

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
