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

const sheet = createStyleSheet({
    body: {
        flexWrap: 'wrap',
        flexDirection: 'row',
        justifyContent: 'space-between',
        paddingLeft: '10vh',
        paddingRight: '10vh',
        backgroundColor: 'gray',
        '@extend': '%absoluteFill'
    },
    filterNone: {
        '@extend': '%cell'
    },
    filterFlipH: {
        filter: $.flipH(),
        '@extend': '%cell'
    },
    filterFlipV: {
        filter: $.flipV(),
        '@extend': '%cell'
    },
    filterTint: {
        filter: $.tint('dodgerblue'),
        '@extend': '%cell'
    },
    text: {
        marginTop: '1vh',
        color: 'white',
        fontSize: '2.5vh',
        textAlign: 'center'
    },
    '%cell': {
        marginTop: '10vh',
        color: 'black',
        fontSize: '3vh',
        textAlign: 'center',
        backgroundColor: 'white',
        width: '30vh'
    }
});

const message = 'Light Source Engine';

const messageMultiline = 'Build React apps on the Raspberry Pi!';

const TextFilterApp = () => jsxs('box', {
    class: sheet.body,
    children: [ jsxs('box', {
        children: [ jsx('text', {
            class: sheet.filterNone,
            children: message
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: none'
        }) ]
    }), jsxs('box', {
        children: [ jsx('text', {
            class: sheet.filterFlipH,
            children: message
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: flipH()'
        }) ]
    }), jsxs('box', {
        children: [ jsx('text', {
            class: sheet.filterFlipV,
            children: message
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: flipV()'
        }) ]
    }), jsxs('box', {
        children: [ jsx('text', {
            class: sheet.filterTint,
            children: message
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: tint(\'dodgerblue\')'
        }) ]
    }), jsxs('box', {
        children: [ jsx('text', {
            class: sheet.filterNone,
            children: messageMultiline
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: none'
        }) ]
    }), jsxs('box', {
        children: [ jsx('text', {
            class: sheet.filterFlipH,
            children: messageMultiline
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: flipH()'
        }) ]
    }), jsxs('box', {
        children: [ jsx('text', {
            class: sheet.filterFlipV,
            children: messageMultiline
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: flipV()'
        }) ]
    }), jsxs('box', {
        children: [ jsx('text', {
            class: sheet.filterTint,
            children: messageMultiline
        }), jsx('text', {
            class: sheet.text,
            children: 'filter: tint(\'dodgerblue\')'
        }) ]
    }) ]
});

letThereBeLight(jsx(TextFilterApp, {}), {
    scene: {
        fullscreen: false
    }
});
