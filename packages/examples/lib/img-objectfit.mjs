/*
 * Light Source Engine
 * Copyright (C) 2019-2020 Daniel Anderson <dan.anderson.oss@gmail.com>.
 *
 * This source code is licensed under the MIT License.
 *
 * License Text:    https://github.com/lightsourceengine/LightSourceEngine/blob/master/LICENSE
 * Original Source: https://github.com/lightsourceengine/LightSourceEngine
 */

import { createStyleSheet } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import React from 'react';

const background = 'resource/pexels-emiliano-arano-1295138.jpg';

const sheet = createStyleSheet({
    body: {
        flexWrap: 'wrap',
        flexDirection: 'row',
        justifyContent: 'space-between',
        padding: '10vh',
        backgroundImage: background,
        '@extend': '%absoluteFill'
    },
    objectFitContain: {
        objectFit: 'contain',
        '@extend': '%cell'
    },
    objectFitFill: {
        objectFit: 'fill',
        '@extend': '%cell'
    },
    objectFitCover: {
        objectFit: 'cover',
        '@extend': '%cell'
    },
    objectFitScaleDown: {
        objectFit: 'scale-down',
        '@extend': '%cell'
    },
    objectFitNone: {
        objectFit: 'none',
        '@extend': '%cell'
    },
    '%cell': {
        border: '1vh',
        borderColor: 'red',
        '@size': '30vh'
    }
});

const ObjectFitApp = () => React.createElement('box', {
    class: sheet.body
}, React.createElement('img', {
    src: background,
    class: sheet.objectFitContain
}), React.createElement('img', {
    src: background,
    class: sheet.objectFitFill
}), React.createElement('img', {
    src: background,
    class: sheet.objectFitCover
}), React.createElement('img', {
    src: background,
    class: sheet.objectFitScaleDown
}), React.createElement('img', {
    src: background,
    class: sheet.objectFitNone
}));

letThereBeLight(React.createElement(ObjectFitApp, null), {
    fullscreen: false
});
