/*
 * Light Source Engine
 * Copyright (C) 2019-2021 Daniel Anderson <dan.anderson.oss@gmail.com>.
 *
 * This source code is licensed under the MIT License.
 *
 * License Text:    https://github.com/lightsourceengine/LightSourceEngine/blob/master/LICENSE
 * Original Source: https://github.com/lightsourceengine/LightSourceEngine
 */

import { jsx, jsxs } from '@lse/react/jsx-runtime';

import { createStyleSheet } from '@lse/core';

import { letThereBeLight } from '@lse/react';

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

const BorderRadiusApp = () => jsxs('box', {
    class: sheet.body,
    children: [ jsx('box', {
        class: sheet.borderRadius
    }), jsx('box', {
        class: sheet.borderRadiusTopLeft
    }), jsx('box', {
        class: sheet.borderRadiusTopRight
    }), jsx('box', {
        class: sheet.borderRadiusBottomLeft
    }), jsx('box', {
        class: sheet.borderRadiusBottomRight
    }) ]
});

letThereBeLight(jsx(BorderRadiusApp, {}), {
    fullscreen: false
});
