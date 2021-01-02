/*
 * Light Source Engine
 * Copyright (C) 2019-2020 Daniel Anderson <dan.anderson.oss@gmail.com>.
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

const TextAlignApp = () => jsxs('box', {
    class: sheet.body,
    children: [ jsx('text', {
        class: sheet.maxLinesTextAlignLeft,
        children: sampleTextSmall
    }), jsx('text', {
        class: sheet.maxLinesTextAlignCenter,
        children: sampleTextSmall
    }), jsx('text', {
        class: sheet.maxLinesTextAlignRight,
        children: sampleTextSmall
    }), jsx('text', {
        class: sheet.textAlignLeft,
        children: sampleText
    }), jsx('text', {
        class: sheet.textAlignCenter,
        children: sampleText
    }), jsx('text', {
        class: sheet.textAlignRight,
        children: sampleText
    }), jsx('text', {
        class: sheet.ellipsisTextAlignLeft,
        children: sampleText
    }), jsx('text', {
        class: sheet.ellipsisTextAlignCenter,
        children: sampleText
    }), jsx('text', {
        class: sheet.ellipsisTextAlignRight,
        children: sampleText
    }), jsx('text', {
        class: sheet.textAlignLeft,
        children: sampleTextNewline
    }), jsx('text', {
        class: sheet.textAlignCenter,
        children: sampleTextNewline
    }), jsx('text', {
        class: sheet.textAlignRight,
        children: sampleTextNewline
    }) ]
});

letThereBeLight(jsx(TextAlignApp, {}), {
    fullscreen: false
});
