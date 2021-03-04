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

import { createStyleSheet, $ } from '@lse/core';

import { letThereBeLight } from '@lse/react';

const background = 'resource/pexels-emiliano-arano-1295138.jpg';

const sheet = createStyleSheet({
    body: {
        flexWrap: 'wrap',
        flexDirection: 'row',
        justifyContent: 'space-between',
        padding: '10vh',
        backgroundColor: 'gray',
        '@extend': '%absoluteFill'
    },
    filterNone: {
        objectFit: 'contain',
        '@extend': '%cell'
    },
    filterFlipH: {
        objectFit: 'contain',
        filter: $.flipH(),
        '@extend': '%cell'
    },
    filterFlipV: {
        objectFit: 'contain',
        filter: $.flipV(),
        '@extend': '%cell'
    },
    filterTint: {
        objectFit: 'contain',
        filter: $.tint('red'),
        '@extend': '%cell'
    },
    text: {
        marginTop: '1vh',
        color: 'white',
        fontSize: '2.5vh',
        textAlign: 'center'
    },
    '%cell': {
        backgroundColor: 'lightgray',
        '@size': '30vh'
    }
});

const ImageFilterApp = () => jsxs('box', {
    class: sheet.body,
    children: [ jsxs('box', {
        children: [ jsx('img', {
            src: background,
            class: sheet.filterNone
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: none'
        }) ]
    }), jsxs('box', {
        children: [ jsx('img', {
            src: background,
            class: sheet.filterFlipH
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: flipH()'
        }) ]
    }), jsxs('box', {
        children: [ jsx('img', {
            src: background,
            class: sheet.filterFlipV
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: flipV()'
        }) ]
    }), jsxs('box', {
        children: [ jsx('img', {
            src: background,
            class: sheet.filterTint
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: tint(\'red\')'
        }) ]
    }) ]
});

letThereBeLight(jsx(ImageFilterApp, {}), {
    scene: {
        fullscreen: false
    }
});
