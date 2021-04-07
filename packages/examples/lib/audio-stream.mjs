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

import { createStyleSheet, stage, AudioDecoderType } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import { useState, useEffect } from 'react';

const sheet = createStyleSheet({
    body: {
        backgroundColor: '#8d99ae',
        padding: 20,
        '@extend': '%absoluteFill'
    },
    label: {
        fontWeight: 'bold',
        color: '#2b2d42',
        fontSize: 24
    }
});

const StreamingAudioApp = () => {
    const [message, setMessage] = useState('Loading background music...');
    useEffect(() => {
        let music;
        if (stage.audio.stream.hasDecoder(AudioDecoderType.OGG)) {
            music = stage.audio.stream.add('resource/bensound-ukulele.ogg');
        } else if (stage.audio.stream.hasDecoder(AudioDecoderType.MP3)) {
            music = stage.audio.stream.add('resource/bensound-ukulele.mp3');
        }
        if (music) {
            music.on('status', event => {
                if (event.error) {
                    setMessage('Error loading background music.');
                } else {
                    music.play();
                    setMessage('Playing background music from file.');
                }
            });
        } else {
            setMessage('No mp3 or ogg decoder available.');
        }
    }, []);
    return jsx('box', {
        class: sheet.body,
        children: jsx('text', {
            class: sheet.label,
            children: message
        })
    });
};

letThereBeLight(jsx(StreamingAudioApp, {}), {
    scene: {
        fullscreen: false
    }
});
