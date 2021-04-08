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
    textOverflowEllipsis: {
        textOverflow: 'ellipsis',
        '@extend': '%cell'
    },
    textOverflowClip: {
        textOverflow: 'clip',
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

const messageMultiline = 'Cras mauris nisl, pharetra a dapibus et, gravida vitae velit. Vestibulum posuere faucibus odio et ultrices. Ut egestas hendrerit magna a commodo. Mauris commodo nisl sit amet euismod tristique. Etiam varius libero a pharetra eleifend. Duis commodo euismod erat, sed vestibulum sapien commodo ac.';

const Column = ({exampleClass, style, children}) => jsxs('box', {
    class: sheet.container,
    children: [ jsx('text', {
        class: sheet.header,
        children
    }), jsx('text', {
        class: exampleClass,
        style,
        children: messageMultiline
    }) ]
});

const TextTruncateApp = () => jsxs('box', {
    class: sheet.body,
    children: [ jsx(Column, {
        exampleClass: sheet.textOverflowEllipsis,
        style: {
            maxLines: 2
        },
        children: 'maxLines: 2, textOverflow: \'ellipsis\''
    }), jsx(Column, {
        exampleClass: sheet.textOverflowClip,
        style: {
            maxLines: 2
        },
        children: 'maxLines: 2, textOverflow: \'clip\''
    }) ]
});

letThereBeLight(jsx(TextTruncateApp, {}), {
    scene: {
        fullscreen: false
    }
});
