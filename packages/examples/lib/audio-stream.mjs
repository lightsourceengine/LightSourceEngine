/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import { jsx } from '@lse/react/jsx-runtime';

import { Style, stage, Constants } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import { useState, useEffect } from 'react';

const sheet = Style.createStyleSheet({
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
        if (stage.audio.stream.hasDecoder(Constants.AudioDecoderType.OGG)) {
            music = stage.audio.stream.add('resource/bensound-ukulele.ogg');
        } else if (stage.audio.stream.hasDecoder(Constants.AudioDecoderType.MP3)) {
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
