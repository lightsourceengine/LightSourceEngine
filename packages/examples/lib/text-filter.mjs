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

import { Style } from '@lse/core';

import { letThereBeLight } from '@lse/react';

const sheet = Style.createStyleSheet({
    filterNone: {
        '@extend': '%cell'
    },
    filterFlipH: {
        filter: Style.flipH(),
        '@extend': '%cell'
    },
    filterFlipV: {
        filter: Style.flipV(),
        '@extend': '%cell'
    },
    filterFlipHV: {
        filter: [ Style.flipH(), Style.flipV() ],
        '@extend': '%cell'
    },
    filterTint: {
        filter: Style.tint('dodgerblue'),
        '@extend': '%cell'
    },
    body: {
        padding: '4vh',
        flexDirection: 'row',
        flexWrap: 'wrap',
        justifyContent: 'space-between',
        backgroundColor: '#264653',
        '@extend': '%absoluteFill'
    },
    header: {
        padding: '2vh',
        color: '#264653',
        backgroundColor: 'white',
        fontSize: '2.5vh'
    },
    container: {
        width: '49%',
        marginBottom: '4vh',
        border: 2,
        borderColor: 'white'
    },
    '%cell': {
        textAlign: 'center',
        fontSize: '2.5vh',
        color: 'lightgray',
        padding: '2vh',
        borderTop: 2,
        borderColor: 'white'
    }
});

const message = 'Light Source Engine';

const messageMultiline = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam dignissim at metus in finibus.';

const Column = ({exampleClass, children}) => jsxs('box', {
    class: sheet.container,
    children: [ jsx('text', {
        class: sheet.header,
        children
    }), jsx('text', {
        class: exampleClass,
        children: message
    }), jsx('text', {
        class: exampleClass,
        children: messageMultiline
    }) ]
});

const TextFilterApp = () => jsxs('box', {
    class: sheet.body,
    children: [ jsx(Column, {
        exampleClass: sheet.filterNone,
        children: 'filter: none'
    }), jsx(Column, {
        exampleClass: sheet.filterFlipH,
        children: 'filter: flipH()'
    }), jsx(Column, {
        exampleClass: sheet.filterFlipV,
        children: 'filter: flipV()'
    }), jsx(Column, {
        exampleClass: sheet.filterFlipHV,
        children: 'filter: [ flipH(), flipV() ]'
    }), jsx(Column, {
        exampleClass: sheet.filterTint,
        children: 'filter: tint(\'dodgerblue\')'
    }) ]
});

letThereBeLight(jsx(TextFilterApp, {}), {
    scene: {
        fullscreen: false
    }
});
