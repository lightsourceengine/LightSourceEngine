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
        justifyContent: 'flex-start',
        backgroundColor: '#264653',
        '@extend': '%absoluteFill'
    },
    textAlignLeft: {
        textAlign: 'left',
        '@extend': '%label'
    },
    textAlignCenter: {
        textAlign: 'center',
        '@extend': '%label'
    },
    textAlignRight: {
        textAlign: 'right',
        '@extend': '%label'
    },
    ellipsisTextAlignLeft: {
        '@extend': [ 'textAlignLeft', '%ellipsis' ]
    },
    ellipsisTextAlignCenter: {
        '@extend': [ 'textAlignCenter', '%ellipsis' ]
    },
    ellipsisTextAlignRight: {
        '@extend': [ 'textAlignRight', '%ellipsis' ]
    },
    maxLinesTextAlignLeft: {
        maxLines: 1,
        '@extend': 'textAlignLeft'
    },
    maxLinesTextAlignCenter: {
        maxLines: 1,
        '@extend': 'textAlignCenter'
    },
    maxLinesTextAlignRight: {
        maxLines: 1,
        '@extend': 'textAlignRight'
    },
    '%ellipsis': {
        maxLines: 1,
        textOverflow: 'ellipsis'
    },
    '%label': {
        fontSize: 24,
        fontWeight: 'bold',
        color: 'lightgray',
        width: '50vh',
        border: 2,
        borderColor: 'white',
        padding: 20,
        marginRight: 10,
        marginBottom: 10
    }
});

const sampleText = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam dignissim at metus in finibus. Duis pharetra lobortis ultrices. Donec ullamcorper elementum ultricies.';

const sampleTextSmall = '   Hello, world!   ';

const sampleTextNewline = '1) Eat\n2) Sleep\n3) Conquer\n4) Repeat';

const TextAlignApp = () => React.createElement('box', {
    class: sheet.body
}, React.createElement('text', {
    class: sheet.maxLinesTextAlignLeft
}, sampleTextSmall), React.createElement('text', {
    class: sheet.maxLinesTextAlignCenter
}, sampleTextSmall), React.createElement('text', {
    class: sheet.maxLinesTextAlignRight
}, sampleTextSmall), React.createElement('text', {
    class: sheet.textAlignLeft
}, sampleText), React.createElement('text', {
    class: sheet.textAlignCenter
}, sampleText), React.createElement('text', {
    class: sheet.textAlignRight
}, sampleText), React.createElement('text', {
    class: sheet.ellipsisTextAlignLeft
}, sampleText), React.createElement('text', {
    class: sheet.ellipsisTextAlignCenter
}, sampleText), React.createElement('text', {
    class: sheet.ellipsisTextAlignRight
}, sampleText), React.createElement('text', {
    class: sheet.textAlignLeft
}, sampleTextNewline), React.createElement('text', {
    class: sheet.textAlignCenter
}, sampleTextNewline), React.createElement('text', {
    class: sheet.textAlignRight
}, sampleTextNewline));

letThereBeLight(React.createElement(TextAlignApp, null), {
    fullscreen: false
});
