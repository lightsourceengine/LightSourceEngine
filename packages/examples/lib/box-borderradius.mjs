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

const sheet = createStyleSheet({
    body: {
        padding: '10vh',
        flexDirection: 'row',
        flexWrap: 'wrap',
        justifyContent: 'space-between',
        backgroundColor: '#264653',
        '@extend': '%absoluteFill'
    },
    borderRadius: {
        backgroundColor: '#2a9d8f',
        borderRadius: '3vh',
        '@extend': '%size'
    },
    borderRadiusTopLeft: {
        backgroundColor: '#e9c46a',
        borderRadiusTopLeft: '3vh',
        '@extend': '%size'
    },
    borderRadiusTopRight: {
        backgroundColor: '#f4a261',
        borderRadiusTopRight: '3vh',
        '@extend': '%size'
    },
    borderRadiusBottomLeft: {
        backgroundColor: '#e76f51',
        borderRadiusBottomLeft: '3vh',
        '@extend': '%size'
    },
    borderRadiusBottomRight: {
        backgroundColor: '#2a9d8f',
        borderRadiusBottomRight: '3vh',
        '@extend': '%size'
    },
    '%size': {
        '@size': '30vh'
    }
});

const BorderRadiusApp = () => React.createElement('box', {
    class: sheet.body
}, React.createElement('box', {
    class: sheet.borderRadius
}), React.createElement('box', {
    class: sheet.borderRadiusTopLeft
}), React.createElement('box', {
    class: sheet.borderRadiusTopRight
}), React.createElement('box', {
    class: sheet.borderRadiusBottomLeft
}), React.createElement('box', {
    class: sheet.borderRadiusBottomRight
}));

letThereBeLight(React.createElement(BorderRadiusApp, null), {
    fullscreen: false
});
