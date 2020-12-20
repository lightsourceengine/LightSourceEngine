/*
 * Light Source Engine
 * Copyright (C) 2019-2020 Daniel Anderson <dan.anderson.oss@gmail.com>.
 *
 * This source code is licensed under the MIT License.
 *
 * License Text:    https://github.com/lightsourceengine/LightSourceEngine/blob/master/LICENSE
 * Original Source: https://github.com/lightsourceengine/LightSourceEngine
 */

import { createStyleSheet, stage, AudioDecoderType } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import React, { useState, useEffect } from 'react';

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
    const [loadingStatus, setLoadingStatus] = useState('loading');
    useEffect(() => {
        let path;
        if (stage.audio.stream.hasDecoder(AudioDecoderType.OGG)) {
            path = 'resource/bensound-ukulele.ogg';
        } else if (stage.audio.stream.hasDecoder(AudioDecoderType.MP3)) {
            path = 'resource/bensound-ukulele.mp3';
        } else {
            setLoadingStatus('not ready');
            return;
        }
        stage.audio.addStream(path).once('status', event => {
            if (event.target.isReady()) {
                event.target.play();
                setLoadingStatus('ready');
            } else {
                setLoadingStatus('not ready');
                console.log(event.error);
            }
        });
    });
    return React.createElement('box', {
        class: sheet.body
    }, React.createElement('text', {
        class: sheet.label
    }, `Background music is ${loadingStatus}.`));
};

letThereBeLight(React.createElement(StreamingAudioApp, null), {
    fullscreen: false
});
